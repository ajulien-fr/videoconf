#include "user.h"
#include "window.h"

void
make_preview(VideoConfData *data)
{
	UserData *user;
	WindowData *window;

	user = g_malloc0(sizeof(UserData));
	user->nick = "preview";

	window = g_malloc0(sizeof(WindowData));
	window->videoconf = data;
	window->user = user;

	make_window(window);

	user->winhandle = window->winhandle;
	user->sink = make_sink(data->ph, user->winhandle, FALSE);

	gst_bin_add(GST_BIN(data->pipeline), user->sink);
	
	gst_element_set_state(user->sink, GST_STATE_PLAYING);
	
	gst_element_link(data->tee, user->sink);
}

