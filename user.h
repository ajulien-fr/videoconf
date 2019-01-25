#ifndef __USER_H__
#define __USER_H__

#include "videoconf.h"

typedef struct
{
	int from;
	gchar *nick;
	gchar *server;
	VideoConfData *videoconf;
	FsParticipant *part;
	FsStream *stream;
	gchar *cand[2];
	GtkWidget *window;
	guintptr winhandle;
	GstElement *sink;
}UserData;

void add_user(UserData *data);

#endif

