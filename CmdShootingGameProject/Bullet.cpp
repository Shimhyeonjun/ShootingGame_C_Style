#include "Bullet.h"
#include <string.h>

const char* getShapePlayerBullet(int bulletCode)
{
	const char* shape = nullptr;
	
	switch (bulletCode)
	{
	case CODE_PLAYER_BULLET:
		shape = SHAPE_PLAYER_BULLET;
		break;
	}

	return shape;
}

const char* getShapeEnemyBullet(int bulletCode)
{
	const char* shape = nullptr;

	switch (bulletCode)
	{
	case CODE_ENEMY_BULLET1:
		shape = SHAPE_ENEMY_BULLET1;
		break;

	case CODE_ENEMY_BULLET2:
		shape = SHAPE_ENEMY_BULLET2;
		break;
	}
	return shape;
}

void makeBulletOfPlayerObj(int numOfBullet, Bullet* bulletsOfPlayer, Player* obj)
{
	const char* shape = nullptr;

	for (int i = 0; i < numOfBullet; ++i)
	{
		if (!bulletsOfPlayer[i].alive)
		{
			shape = getShapePlayerBullet(obj->bCode);

			memcpy(bulletsOfPlayer[i].object, shape, strlen(shape) + 1);

			bulletsOfPlayer[i].iPosX		= obj->iPosX + strlen(obj->object) / 2;
			bulletsOfPlayer[i].iPosY		= obj->iPosY - 1;
			bulletsOfPlayer[i].mvFrame		= g_Frames;
			bulletsOfPlayer[i].code			= obj->bCode;
			bulletsOfPlayer[i].mvDelay		= obj->bMvDelay;
			bulletsOfPlayer[i].damage		= obj->bDamege;
			bulletsOfPlayer[i].alive		= true;
			break;
		}
	}
}

void moveBulletsOfPlayerObj(int numOfBullet, Bullet* bulletsOfPlayer)
{
	for (int i = 0; i < numOfBullet; ++i)
	{
		if (!bulletsOfPlayer[i].alive)
			continue;

		if (bulletsOfPlayer[i].iPosX < 0 || bulletsOfPlayer[i].iPosX > GAME_SCREEN_WIDTH - 1
				|| bulletsOfPlayer[i].iPosY < 0 || bulletsOfPlayer[i].iPosY > GAME_SCREEN_HIGHT - 1)
		{
			bulletsOfPlayer[i].alive = false;
		}

		else
		{
			int mvFrame = bulletsOfPlayer[i].mvFrame + bulletsOfPlayer[i].mvDelay;

			if (g_Frames > mvFrame)
			{
				bulletsOfPlayer[i].mvFrame = mvFrame;
				--bulletsOfPlayer[i].iPosY;
			}
		}
	}

}

void makeBulletOfEnemyObj(int numOfBullet, Bullet* bulletsOfEnemy, int numOfEnemy, Enemy* obj)
{
	const int SIZE_BULLET_PER_ENEMY = 10;	

	int bulletPosX[SIZE_BULLET_PER_ENEMY];
	int bulletPosY[SIZE_BULLET_PER_ENEMY];

	for (int i = 0; i < numOfEnemy; ++i)
	{
		if (obj[i].hp <= 0 || !obj[i].isRpw)
			continue;

		int atFrame = obj[i].atFrame + obj[i].atDelay;

		if (g_Frames < atFrame)
			continue;

		obj[i].atFrame = atFrame;
		
		int makeBulleCount = 0;

		const char* shape = getShapeEnemy(obj[i].code);

		switch (obj[i].code)
		{
		case CODE_ENEMY1:
			makeBulleCount = 1;			
			bulletPosX[0] = obj[i].iPosX + strlen(shape) / 2 + 1;
			bulletPosY[0] = obj[i].iPosY + 1;
			break;

		case CODE_ENEMY2:
			makeBulleCount = 1;
			bulletPosX[0] = obj[i].iPosX + strlen(shape) / 2 + 1;
			bulletPosY[0] = obj[i].iPosY + 1;
			break;

		case CODE_ENEMY3:			
			makeBulleCount = 2;
			bulletPosX[0] = obj[i].iPosX + 1;
			bulletPosX[1] = obj[i].iPosX + strlen(shape) - 1;

			bulletPosY[0] = obj[i].iPosY + 1;
			bulletPosY[1] = obj[i].iPosY + 1;
			break;

		case CODE_ENEMY4:
			makeBulleCount = 2;
			bulletPosX[0] = obj[i].iPosX + 1;
			bulletPosX[1] = obj[i].iPosX + strlen(shape) - 1;

			bulletPosY[0] = obj[i].iPosY + 1;
			bulletPosY[1] = obj[i].iPosY + 1;
			break;
		}

		for (int j = 0; j < makeBulleCount; ++j)
		{
			for (int k = 0; k < numOfBullet; ++k)
			{
				if (!bulletsOfEnemy[k].alive)
				{
					const char* bShape = getShapeEnemyBullet(obj[i].bCode);

					memset(&bulletsOfEnemy[k], 0, sizeof(Bullet));

					memcpy(bulletsOfEnemy[k].object, bShape, strlen(bShape) + 1);

					bulletsOfEnemy[k].code = obj[i].bCode;
					bulletsOfEnemy[k].mvFrame = g_Frames;
					bulletsOfEnemy[k].mvDelay = obj[i].bMvDelay;
					bulletsOfEnemy[k].damage = obj[i].bDemage;
					bulletsOfEnemy[k].iPosX = bulletPosX[j];
					bulletsOfEnemy[k].iPosY = bulletPosY[j];
					bulletsOfEnemy[k].alive = true;

					break;
				}
			}
		}
	}
}

void moveBulletsOfEnemyObj(int numOfBullet, Bullet* bulletsOfEnemy)
{
	for (int i = 0; i < numOfBullet; ++i)
	{
		if (!bulletsOfEnemy[i].alive)
			continue;

		if (bulletsOfEnemy[i].iPosX < 0 || bulletsOfEnemy[i].iPosX > GAME_SCREEN_WIDTH - 1
			|| bulletsOfEnemy[i].iPosY < 0 || bulletsOfEnemy[i].iPosY > GAME_SCREEN_HIGHT - 1)
		{
			bulletsOfEnemy[i].alive = false;
		}
		else
		{
			int mvFrame = bulletsOfEnemy[i].mvFrame + bulletsOfEnemy[i].mvDelay;

			if (g_Frames > mvFrame)
			{
				bulletsOfEnemy[i].mvFrame = mvFrame;
				++bulletsOfEnemy[i].iPosY;				
			}
		}
	}
}

void detectCollisonPlayer(int numOfBullet, Bullet* bulletsOfEnemy, Player* obj)
{	
	if (obj->state == STATE_PLAYER_INVINCIBILITY && obj->stateFrame > g_Frames)
		return;
	
	for (int i = 0; i < numOfBullet; ++i)
	{		
		if (!bulletsOfEnemy[i].alive)
			continue;

		if (bulletsOfEnemy[i].iPosX < obj->iPosX || bulletsOfEnemy[i].iPosX > (obj->iPosX + strlen(obj->object)))
			continue;

		if (bulletsOfEnemy[i].iPosY < obj->iPosY || bulletsOfEnemy[i].iPosY > obj->iPosY )
			continue;
		
		obj->hp = obj->hp - bulletsOfEnemy[i].damage;

		if (obj->hp < 0)
			obj->hp = 0;
		
		bulletsOfEnemy[i].alive = false;

		obj->object[bulletsOfEnemy[i].iPosX - obj->iPosX] = SHAPE_ATTACKED;
		obj->state			= STATE_PLAYER_INVINCIBILITY;
		obj->stateFrame		= g_Frames + FRAME_INVINCIBILITY;
	}
}

void detectCollisonEnemy(int numOfBullet, Bullet* bulletsOfPlayer, int numOfEnemy, Enemy* obj, Player* player)
{
	for (int i = 0; i < numOfBullet; ++i)
	{
		if (!bulletsOfPlayer[i].alive)
			continue;

		for (int j = 0; j < numOfEnemy; ++j)
		{
			if (obj[j].hp <= 0 || !obj[j].isRpw)
				continue;

			if (bulletsOfPlayer[i].iPosX < obj[j].iPosX || bulletsOfPlayer[i].iPosX > (obj[j].iPosX + strlen(obj[j].object)))
				continue;

			if (bulletsOfPlayer[i].iPosY < obj[j].iPosY || bulletsOfPlayer[i].iPosY > obj[j].iPosY)
				continue;

			obj[j].hp = obj[j].hp - bulletsOfPlayer[i].damage;

			if (obj->hp < 0)
				obj->hp = 0;

			if (obj[j].hp < bulletsOfPlayer[i].damage)
			{
				obj[j].hp = 0;
				player->totalScore += obj->score;
			}
						
			bulletsOfPlayer[i].alive = false;

			obj[j].object[bulletsOfPlayer[i].iPosX - obj[j].iPosX] = SHAPE_ATTACKED;
			obj[j].isAttacked = true;
		}
	}
}
