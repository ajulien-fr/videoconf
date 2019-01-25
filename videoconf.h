#ifndef __VIDEOCONF_H__
#define __VIDEOCONF_H__

#define PNAME "videoconf"
#define PDESC "video conference plugin"
#define PVERSION "0.1"

#include "hexchat-plugin.h"

#include <gst/gst.h>
#include <gst/video/videooverlay.h>

#include <farstream/fs-conference.h>

#include <gtk/gtk.h>

#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h> // for GDK_WINDOW_XID
#endif
#ifdef GDK_WINDOWING_WIN32
#include <gdk/gdkwin32.h> // for GDK_WINDOW_HWND
#endif

enum { FOUNDATION, COMPONENT_ID, IP, PORT };
enum { RTP, RTCP, CODEC_ID, CODEC_CLOCK_RATE, CODEC_CHANNELS };
enum { ACCEPT, ACCEPTED };

typedef struct _VideoConfData VideoConfData;
struct _VideoConfData
{
	hexchat_plugin *ph;
	gchar *ip;
	GstElement *pipeline;
	GstElement *conf;
	GstElement *src;
	GstElement *tee;
	GstElement *queue;
	GstPad *srcpad;
	FsSession *session;
	GList *users;
};

void print_error(hexchat_plugin *ph, GError *error);

void videoconf_enable(VideoConfData *data);
void videoconf_disable(VideoConfData *data);

void make_conference(VideoConfData *data);

void make_preview(VideoConfData *data);

void add_candidates(VideoConfData *data, gchar *nick, const gchar *server, gchar *string);

GstElement *make_sink(hexchat_plugin *ph, guintptr winhandle, gboolean sync);

void send_request(VideoConfData *data, gchar *nick);
void request_received(VideoConfData *data, gchar *nick);
void accept_request(VideoConfData *data, gchar *nick);
void request_accepted(VideoConfData *data, gchar *nick, gchar *string);

#endif

