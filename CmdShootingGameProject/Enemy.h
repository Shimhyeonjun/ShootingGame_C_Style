#ifndef ENEMY_H
#define ENEMY_H

#include "GameDef.h"

struct Enemy
{
	char object[SIZE_SHAPE_ENEMY];
	int	 code;
	int	 hp;									
	int  iPosX;
	int  iPosY;
	
	bool isAttacked;
	bool isRpw;
	bool canRpw;
	
	int  rpwFrame;

	int  mvFrame;
	int  mvDelay;

	int  atFrame;
	int  atDelay;
	
	int	 bCode;
	int  bMvDelay;
	int  bDemage;	
	
	int	 score;

	int pageNumber;
	__int64 order;	

};

bool isEnemy(int enemyCode);
void checkEnemies(int numOfEnemy, Enemy* enemies);

const char* getShapeEnemy(int enemyCode);
int getBulletCodeOfEnemy(int enemyCode);

void respawnEnemies(int numOfEnemy, Enemy* enemies);
void moveEnemies(int numOfEnemy, Enemy* enemies, Player* player);

#endif 