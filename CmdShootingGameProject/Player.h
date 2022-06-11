#ifndef PLAYER_H
#define PLAYER_H

#include "GameDef.h"

struct Player
{
	char object[SIZE_SHAPE_PLAYER];
	int	hp;
	int iPosX;
	int iPosY;
	
	int mvFrame;
	int mvDelay;							
	
	int atFrame;
	int atDelay;							

	int bCode;
	int bMvDelay;							// ÃÑ¾Ë ¼Óµµ
	int bDamege;							// ÃÑ¾Ë µ¥¹ÌÁö

	int totalScore;

	int state;
	int stateFrame;
};

void checkPlayer(Player* player);
#endif 

