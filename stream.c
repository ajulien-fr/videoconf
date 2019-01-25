#include "stream.h"

static void element_cb(GstBus *bus, GstMessage *message, gpointer userdata);
static void src_pad_added_cb(FsStream *stream, GstPad *srcpad, FsCodec *codec, gpointer userdata);

static void new_local_candidate(UserData *data, FsCandidate *candidate);
static void local_candidates_prepared(UserData *data);

void
make_stream(UserData *data)
{
	GError *error = NULL;
	GstBus *bus;

	bus = gst_pipeline_get_bus(GST_PIPELINE(data->videoconf->pipeline));
	gst_bus_add_signal_watch(bus);
	g_signal_connect(bus, "message::element", G_CALLBACK(element_cb), data);
	gst_object_unref(bus);

	data->stream = fs_session_new_stream(data->videoconf->session, data->part, FS_DIRECTION_BOTH, &error);
	print_error(data->videoconf->ph, error);
	
	fs_stream_set_transmitter(data->stream, "rawudp", 0, 0, &error);
	print_error(data->videoconf->ph, error);
	
	g_signal_connect(data->stream, "src-pad-added", G_CALLBACK(src_pad_added_cb), data);
}

void
src_pad_added_cb(FsStream *stream, GstPad *srcpad, FsCodec *codec, gpointer userdata)
{
	UserData *data = (UserData *)userdata;
	GstPad *sinkpad;

	data->sink = make_sink(data->videoconf->ph, data->winhandle, TRUE);

	gst_bin_add(GST_BIN(data->videoconf->pipeline), data->sink);

	gst_element_set_state(data->sink, GST_STATE_PLAYING);
	
	sinkpad = gst_element_get_static_pad(data->sink, "sink");
	
	gst_pad_link(srcpad, sinkpad);
	
	gst_object_unref(sinkpad);
}

void
element_cb(GstBus *bus, GstMessage *message, gpointer userdata)
{
	UserData *data = (UserData *)userdata;
	const GstStructure *s = gst_message_get_structure(message);
		
	if(gst_structure_has_name(s, "farstream-new-local-candidate"))
	{
		const GValue *val = gst_structure_get_value(s, "candidate");
		FsCandidate *candidate;

		candidate = g_value_get_boxed(val);

		new_local_candidate(data, candidate);
	}

	else if(gst_structure_has_name(s, "farstream-local-candidates-prepared"))
	{
		local_candidates_prepared(data);
	}
}

void
new_local_candidate(UserData *data, FsCandidate *candidate)
{
	if(candidate->component_id == 1) // RTP
	{
		data->cand[RTP] = g_strdup_printf("%s|%d|%s|%d", candidate->foundation, candidate->component_id, data->videoconf->ip, candidate->port);
	}
				
	else if(candidate->component_id == 2) // RTCP
	{
		data->cand[RTCP] = g_strdup_printf("%s|%d|%s|%d", candidate->foundation, candidate->component_id, data->videoconf->ip, candidate->port);
	}
}

void
local_candidates_prepared(UserData *data)
{
	FsCodec *codec = NULL;

	g_object_get(data->videoconf->session, "current-send-codec", &codec, NULL);

	g_print("codec: id: %d encoding_name: %s channels: %d\n", codec->id, codec->encoding_name, codec->channels);

	if(data->from == ACCEPT)
	{
		hexchat_commandf(data->videoconf->ph,
				"QUOTE PRIVMSG %s :VIDEOCONF:OK:%s:%s:%d:%d:%d", data->nick, data->cand[RTP], data->cand[RTCP], codec->id, codec->clock_rate, codec->channels);
	}

	else if(data->from == ACCEPTED)
	{
		hexchat_commandf(data->videoconf->ph,
				"QUOTE PRIVMSG %s :VIDEOCONF:CANDIDATES:%s:%s:%d:%d:%d", data->nick, data->cand[RTP], data->cand[RTCP], codec->id, codec->clock_rate, codec->channels);	

	}
}

