#include "videoconf.h"

GstElement *
make_sink(hexchat_plugin *ph, guintptr winhandle, gboolean sync)
{
	GstElement *bin, *sink;
	GError *error = NULL;
	
	bin = gst_parse_bin_from_description("videoconvert ! videoscale ! videoconvert ! xvimagesink", TRUE, &error);
	print_error(ph, error);
	
	sink = gst_bin_get_by_interface(GST_BIN(bin), GST_TYPE_VIDEO_OVERLAY);
	
	gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(sink), winhandle);
	
	g_object_set(sink, "sync", sync, NULL);
	
	return bin;
}

