#include "videoconf.h"

void
print_error(hexchat_plugin *ph, GError *error)
{
	if(error)
	{
		g_printerr("ERROR: domain:%s code:%d message:%s\n", g_quark_to_string(error->domain), error->code, error->message);
		
		hexchat_printf(ph, "HEXVIDEOCONF: ERROR: domain:%s code:%d message:%s\n", g_quark_to_string(error->domain), error->code, error->message);
	}
}

