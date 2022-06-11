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
	int bMvDelay;							// �Ѿ� �ӵ�
	int bDamege;							// �Ѿ� ������

	int totalScore;

	int state;
	int stateFrame;
};

void checkPlayer(Player* player);
#endif 

