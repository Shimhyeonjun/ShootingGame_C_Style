#pragma once
#ifndef GAME_DEF_H
#define GAME_DEF_H

extern int g_Frames;

/* common */

#define GAME_SCREEN_WIDTH			81
#define GAME_SCREEN_HIGHT			30

#define SHAPE_ATTACKED				' '

#define MAX_STAGE					8
#define MAX_PAGE					5
#define MAX_PAGE_ENEMY				20

#define MAX_STAGE_ENEMY				MAX_PAGE * MAX_PAGE_ENEMY

/* Player */

#define	SIZE_SHAPE_PLAYER			10

#define STATE_PLAYER_NORMAL			101
#define STATE_PLAYER_INVINCIBILITY	102
#define STATE_PLAYER_DEAD			103

#define FRAME_INVINCIBILITY			30

#define CODE_PLAYER_BULLET			301

#define SHAPE_PLAYER_BULLET			"*"

#define SHAPE_PLAYER				"|_0_|"


/* Enemy */
#define SIZE_SHAPE_ENEMY			10

#define CODE_ENEMY1					201
#define CODE_ENEMY2					202
#define CODE_ENEMY3					203
#define CODE_ENEMY4					204

#define SHAPE_ENEMY1				"<=0=>"
#define SHAPE_ENEMY2				"<=v=>"
#define SHAPE_ENEMY3				"[|=0=|]"
#define SHAPE_ENEMY4				"<\\=v=\\>"


#define CODE_ENEMY_BULLET1			501
#define CODE_ENEMY_BULLET2			502

#define SHAPE_ENEMY_BULLET1			"."
#define SHAPE_ENEMY_BULLET2			"v"

/* Bullet */
#define SIZE_SHAPE_BULLET			2

#define SIZE_PLAYER_BULLET			100
#define SIZE_ENEMY_BULLET			200

#endif 


