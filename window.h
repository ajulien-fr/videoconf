#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "videoconf.h"
#include "user.h"

typedef struct
{
	VideoConfData *videoconf;
	UserData *user;
	guintptr winhandle;
}WindowData;

void make_window(WindowData *data);

#endif

