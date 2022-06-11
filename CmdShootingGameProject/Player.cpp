#include "Player.h"
#include <string.h>

void checkPlayer(Player* player)
{
	static bool visible = true;

	if (player->state == STATE_PLAYER_INVINCIBILITY)
	{
		if (player->stateFrame < g_Frames)
		{
			visible = true;

			player->state = STATE_PLAYER_NORMAL;
			player->stateFrame = 0;

			memcpy(player->object, SHAPE_PLAYER, strlen(SHAPE_PLAYER) + 1);
			return;
		}
		else
		{
			if (visible)
				memcpy(player->object, SHAPE_PLAYER, strlen(SHAPE_PLAYER) + 1);
			else
				memcpy(player->object, "    ", strlen(SHAPE_PLAYER) + 1);

			visible = !visible;
		}
	}
}