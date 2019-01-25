#include "window.h"

static void video_realize_cb(GtkWidget *widget, gpointer data);
static void window_destroy_cb(GtkWidget *widget, gpointer data);

void
make_window(WindowData *data)
{
	gchar *name;
	GtkWidget *window, *video;

	if(data->user->server == NULL) name = g_strdup_printf("%s", data->user->nick);
	else name = g_strdup_printf("%s@%s", data->user->nick, data->user->server);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), name);
	g_free(name);
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
	gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(hexchat_get_info(data->videoconf->ph, "gtkwin_ptr")));
	g_signal_connect(window, "destroy", G_CALLBACK(window_destroy_cb), data);
	
	video = gtk_drawing_area_new();
	gtk_widget_set_double_buffered(video, FALSE);
	g_signal_connect(video, "realize", G_CALLBACK(video_realize_cb), data);
	
	gtk_container_add(GTK_CONTAINER(window), video);
	
	gtk_widget_show_all(window);
	
	gtk_widget_realize(video);
}

void
video_realize_cb(GtkWidget *widget, gpointer userdata)
{
	WindowData *data = (WindowData *)userdata;
	
	#ifdef GDK_WINDOWING_X11
	{
		data->winhandle = GDK_WINDOW_XID(gtk_widget_get_window(widget)); // Je get le XID (linux).
	}
	#endif
	
	#ifdef GDK_WINDOWING_WIN32
	{
		data->winhandle = GDK_WINDOW_HWND(gtk_widget_get_window(widget)); // Je get le HWD (microsoft windows).
	}
	#endif
}

void
window_destroy_cb(GtkWidget *widget, gpointer userdata)
{
	WindowData *data = (WindowData *)userdata;
	
	gst_element_set_state(data->user->sink, GST_STATE_NULL);
	
	gst_bin_remove(GST_BIN(data->videoconf->pipeline), data->user->sink);
}

