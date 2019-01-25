#include "user.h"
#include "stream.h"

void
add_user(UserData *data)
{
	GError *error = NULL;

	/* Je dois créer un participant pour cet user */
	data->part = fs_conference_new_participant(FS_CONFERENCE(data->videoconf->conf), &error);
	print_error(data->videoconf->ph, error);

	make_stream(data);	// Je dois créer un stream pour cet user
}

