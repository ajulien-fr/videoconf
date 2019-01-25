#include "videoconf.h"

void
videoconf_enable(VideoConfData *data)
{
	make_conference(data);

	make_preview(data);

	gst_element_set_state(data->pipeline, GST_STATE_PLAYING);

	hexchat_command(data->ph, "MENU ADD \"$NICK/Start Conference\" \"VIDEOCONF start %s\"");
}

