#include "videoconf.h"

static void make_source(VideoConfData *data);
static GstElement *make_video_source(void);
static void make_session(VideoConfData *data);

void
make_conference(VideoConfData *data)
{
	data->pipeline = gst_pipeline_new(NULL);

	data->conf = gst_element_factory_make("fsrtpconference", NULL);
	gst_bin_add(GST_BIN(data->pipeline), data->conf);

	make_source(data);
	
	make_session(data);
}

void
make_source(VideoConfData *data)
{
	GstElement *fakesink;
	
	data->tee = gst_element_factory_make("tee", NULL);
	gst_bin_add(GST_BIN(data->pipeline), data->tee);
	
	fakesink = gst_element_factory_make("fakesink", NULL);
	g_object_set(fakesink, "async", FALSE, "sync", FALSE, NULL);
	
	gst_element_set_state(fakesink, GST_STATE_PLAYING);
	gst_element_set_state(data->tee, GST_STATE_PLAYING);
	
	gst_bin_add(GST_BIN(data->pipeline), fakesink);
	
	gst_element_link(data->tee, fakesink);
	
	data->src = make_video_source();
	gst_bin_add(GST_BIN(data->pipeline), data->src);
	
	gst_element_link(data->src, data->tee);
	
	data->srcpad = gst_element_get_request_pad(data->tee, "src_%u");
}

GstElement *
make_video_source(void)
{
	GstElement *bin, *source, *videoscale;
	GstPad *srcpad;
	
	bin = gst_bin_new(NULL);

	#ifdef G_OS_WIN32
	source = gst_element_factory_make("v4l2src", NULL);
	#endif

	#ifdef G_OS_UNIX
	source = gst_element_factory_make("v4l2src", NULL);
	#endif
	
	gst_bin_add(GST_BIN(bin), source);
	
	videoscale = gst_element_factory_make("videoscale", NULL);
	gst_bin_add(GST_BIN(bin), videoscale);
	
	gst_element_link(source, videoscale);
	
	srcpad = gst_element_get_static_pad(videoscale, "src");
	
	gst_element_add_pad(bin, gst_ghost_pad_new("src", srcpad));
	
	gst_object_unref(srcpad);
	
	return bin;
}

void
make_session(VideoConfData *data)
{
	GError *error = NULL;
	GstPad *srcpad, *sinkpad;
	GList *codecs = NULL;
	GList *l;
	
	data->session = fs_conference_new_session(FS_CONFERENCE(data->conf), FS_MEDIA_TYPE_VIDEO, &error);
	print_error(data->ph, error);
	
	data->queue = gst_element_factory_make("queue", NULL);
	gst_bin_add(GST_BIN(data->pipeline), data->queue);
	
	srcpad = gst_element_get_static_pad(data->queue, "src");
	
	g_object_get(data->session, "sink-pad", &sinkpad, NULL);
	
	gst_pad_link(srcpad, sinkpad);
	
	gst_object_unref(srcpad);
	gst_object_unref(sinkpad);
	
	gst_element_set_state(data->queue, GST_STATE_PLAYING);
	
	sinkpad = gst_element_get_static_pad(data->queue, "sink");
	
	gst_pad_link(data->srcpad, sinkpad);
	
	gst_object_unref(sinkpad);

	g_object_get(data->session, "codecs-without-config", &codecs, NULL);
	fs_session_set_codec_preferences(data->session, codecs, &error);
	print_error(data->ph, error);
	
	for(l = codecs; l != NULL; l = l->next)
	{
		FsCodec *codec = l->data;

		if(!g_strcmp0(codec->encoding_name, "H264"))
		{
			g_print("H264 found!\n");

			fs_session_set_send_codec(data->session, codec, &error);
			print_error(data->ph, error);
		}
	}

	fs_codec_list_destroy(codecs);
}

