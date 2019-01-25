#include "videoconf.h"
#include "user.h"
#include "window.h"

void
send_request(VideoConfData *data, gchar *nick)
{
	hexchat_commandf(data->ph, "QUOTE PRIVMSG %s :VIDEOCONF:START?", nick);
}

void
request_received(VideoConfData *data, gchar *nick)
{
	/* J'ai recu VIDEOCONF:START? */

	hexchat_context *context;
	
	hexchat_commandf(data->ph, "QUERY -nofocus %s", nick);
	
	context = hexchat_find_context(data->ph, NULL, nick);
	
	hexchat_set_context(data->ph, context);
	
	hexchat_printf(data->ph, "videoconf: %s, %s wants to send you his video\n", hexchat_get_info(data->ph, "nick"), nick);
	
	hexchat_command(data->ph, "GUI COLOR 3");
	
	hexchat_command(data->ph, "TRAY -i 8");
}

void
accept_request(VideoConfData *data, gchar *nick)
{
	UserData *user;
	WindowData *window;
	
	user = g_malloc(sizeof(UserData));
	user->from = ACCEPT;
	user->nick = g_strdup(nick); // son nick
	user->server = g_strdup(hexchat_get_info(data->ph, "server")); // le nom du serveur ou cet user se trouve
	user->videoconf = data;

	window = g_malloc0(sizeof(WindowData));
	window->videoconf = data;
	window->user = user;

	make_window(window);

	user->winhandle = window->winhandle;
	add_user(user); // add user to conference

	data->users = g_list_append(data->users, user);
}

void
request_accepted(VideoConfData *data, gchar *nick, gchar *string)
{
	UserData *user;
	WindowData *window;
	GList *l;

	for(l = data->users; l != NULL; l = l->next)
	{
		UserData *user = l->data;

		if((!g_strcmp0(user->nick, nick)) && (!g_strcmp0(user->server, string)))
		{
			user = g_malloc(sizeof(UserData));
			user->from = ACCEPTED;
			user->nick = g_strdup(nick); // son nick
			user->server = g_strdup(hexchat_get_info(data->ph, "server")); // le nom du serveur ou cet user se trouve
			user->videoconf = data;

			window = g_malloc0(sizeof(WindowData));
			window->videoconf = data;
			window->user = user;

			make_window(window);

			user->winhandle = window->winhandle;
			add_user(user); // add user to conference

			data->users = g_list_append(data->users, user);

			add_candidates(data, user->nick, user->server, string);
		}
	}
}

