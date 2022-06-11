#ifndef BULLET_H
#define BULLET_H

#include "GameDef.h"

#include "Player.h"
#include "Enemy.h"

struct Bullet
{
	char object[SIZE_SHAPE_BULLET];
	int code;
	int iPosX;
	int iPosY;

	int mvFrame;
	int mvDelay;

	int damage;
	bool alive;
};

const char* getShapePlayerBullet(int bulletCode);
const char* getShapeEnemyBullet(int bulletCode);

void makeBulletOfPlayerObj(int numOfBullet, Bullet* bulletsOfPlayer, Player* obj);
void makeBulletOfEnemyObj(int numOfBullet, Bullet* bulletsOfEnemy, int numOfEnemy, Enemy* obj);

void moveBulletsOfPlayerObj(int numOfBullet, Bullet* bulletsOfPlayer);
void moveBulletsOfEnemyObj(int numOfBullet, Bullet* bulletsOfEnemy);

void detectCollisonPlayer(int numOfBullet, Bullet* bulletsOfEnemy, Player* obj);
void detectCollisonEnemy(int numOfBullet, Bullet* bulletsOfPlayer, int numOfEnemy, Enemy* obj, Player* player);

#endif

