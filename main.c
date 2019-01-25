#include "videoconf.h"

static int enable = 0;

static int videoconf_cb(char *word[], char *word_eol[], void *userdata);
static int ctcp_cb(char *word[], void *userdata);

void
hexchat_plugin_get_info(char **name, char **desc, char **version, void **reserved)
{
	*name = PNAME;
	*desc = PDESC;
	*version = PVERSION;
}

int
hexchat_plugin_init(hexchat_plugin *plugin_handle, char **plugin_name, char **plugin_desc, char **plugin_version, char *arg)
{
	VideoConfData *data;
	
	/* tell HexChat our info */
	*plugin_name = PNAME;
	*plugin_desc = PDESC;
	*plugin_version = PVERSION;
	
	gst_init(NULL, NULL);
	
	data = g_malloc0(sizeof(VideoConfData));
	
	data->ph = plugin_handle;
	data->ip = g_strdup("127.0.0.1"); // FOR TEST
	
	hexchat_hook_command(data->ph, "VIDEOCONF", HEXCHAT_PRI_NORM, videoconf_cb, "usage: VIDEOCONF [enable|disable|start <nick>|accept <nick>]", data);
	hexchat_hook_print(data->ph, "CTCP Generic", HEXCHAT_PRI_NORM, ctcp_cb, data);
	
	hexchat_print(data->ph, "videoconf plugin loaded\n");
	
	return 1;	// return 1 for success
}

int
videoconf_cb(char *word[], char *word_eol[], void *userdata)
{
	VideoConfData *data = (VideoConfData *)userdata;
	
	if(!g_ascii_strcasecmp(word[2], "enable"))
	{
		if(enable == 1) hexchat_print(data->ph, "videoconf: already enabled\n");
		
		else
		{
			if(data->ip == NULL)
			{
				hexchat_print(data->ph, "videoconf: you have to set \"Get my address from the IRC server\" from \"Settings\" and reconnect\n");
			}
			
			else
			{
				videoconf_enable(data);
				
				enable = 1;
			}
		}
	}
	
	else if(!g_ascii_strcasecmp(word[2], "disable"))
	{
		if(enable == 0) hexchat_print(data->ph, "videoconf: already disabled\n");
		
		else
		{
			videoconf_disable(data);
			
			enable = 0;
		}
	}
	
	else if(!g_ascii_strcasecmp(word[2], "start"))
	{
		if(enable == 0) hexchat_print(data->ph, "videoconf: need to be enabled\n");
		
		else send_request(data, word[3]);
	}
	
	else if(!g_ascii_strcasecmp(word[2], "accept"))
	{
		if(enable == 0) hexchat_print(data->ph, "videoconf: need to be enabled\n");
		
		else accept_request(data, word[3]);
	}
	
	else
	{
		hexchat_print(data->ph, "videoconf: unknown command\n");
		hexchat_command(data->ph, "HELP VIDEOCONF");
	}
	
	return HEXCHAT_EAT_ALL;
}

static int
ctcp_cb(char *word[], void *userdata)
{
	VideoConfData *data = (VideoConfData *)userdata;
	
	if(enable == 1)
	{
		if(g_str_has_prefix(word[1], "VIDEOCONF:OK:"))
		{
			request_accepted(data, word[2], &word[1][13]);
			
			return HEXCHAT_EAT_ALL;
		}
		
		else if(!g_strcmp0(word[1], "VIDEOCONF:START?"))
		{
			request_received(data, word[2]);
			
			return HEXCHAT_EAT_ALL;
		}

		else if(g_str_has_prefix(word[1], "VIDEOCONF:CANDIDATES:"))
		{
			add_candidates(data, word[2], hexchat_get_info(data->ph, "server"), &word[1][21]);

			return HEXCHAT_EAT_ALL;
		}
	}
	
	return HEXCHAT_EAT_NONE;	// don't eat this event, let other plugins and HexChat see it too
}

