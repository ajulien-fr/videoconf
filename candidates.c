#include "videoconf.h"
#include "stream.h"

static FsCandidate *get_candidate_from_string(gchar *string);

void
add_candidates(VideoConfData *data, gchar *nick, const gchar *server, gchar *string)
{
	GList *l;
	gchar **str;
	FsCodec *codec;
	GList *codecs = NULL;
	FsCandidate *cand[2];
	GList *cands = NULL;
	GError *error = NULL;

	for(l = data->users; l != NULL; l = l->next)
	{
		UserData *user = l->data;

		if((!g_strcmp0(user->nick, nick)) && (!g_strcmp0(user->server, server)))
		{
			str = g_strsplit(string, ":", 5);

			codec = fs_codec_new(g_ascii_strtoll(str[CODEC_ID], NULL, 10), "H264", FS_MEDIA_TYPE_VIDEO, g_ascii_strtoll(str[CODEC_CLOCK_RATE], NULL, 10));
			fs_codec_add_optional_parameter(codec, "channels", str[CODEC_CHANNELS]);
			codecs = g_list_append(codecs, codec);
			fs_stream_set_remote_codecs(user->stream, codecs, &error);
			print_error(data->ph, error);

			cand[RTP] = get_candidate_from_string(str[RTP]);
			cand[RTCP] = get_candidate_from_string(str[RTCP]);

			g_strfreev(str);

			cands = g_list_prepend(cands, cand[RTP]);
			cands = g_list_prepend(cands, cand[RTCP]);

			fs_stream_force_remote_candidates(user->stream, cands, &error);
			print_error(data->ph, error);
		
			fs_candidate_list_destroy(cands);
		}
	
	}	
}

FsCandidate *
get_candidate_from_string(gchar *string)
{
	gchar **str;
	FsCandidate *cand;
	
	str = g_strsplit(string, "|", 4);
	
	cand = fs_candidate_new_full(str[FOUNDATION],
								 g_ascii_strtoll(str[COMPONENT_ID], NULL, 10), 
								 str[IP],
								 g_ascii_strtoll(str[PORT], NULL, 10), 
								 NULL, 0, 0, 0, 0, NULL, NULL, 0);
	
	g_strfreev(str);
	
	return cand;
}

