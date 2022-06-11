#include "Player.h"
#include "Enemy.h"
#include <string.h>

bool isEnemy(int enemyCode)
{
	bool result = false;

	switch (enemyCode)
	{
	case CODE_ENEMY1:
	case CODE_ENEMY2:
	case CODE_ENEMY3:
	case CODE_ENEMY4:
		if (getShapeEnemy(enemyCode) != nullptr)
			result = true;
		break;
	}

	return result;
}

void checkEnemies(int numOfEnemy, Enemy* enemies)
{
	const char* shape = nullptr;

	for (int i = 0; i < numOfEnemy; ++i)
	{
		if (enemies[i].hp <= 0 || !enemies[i].isRpw)
			continue;

		if (enemies[i].isAttacked)
		{
			enemies[i].isAttacked = false;

			shape = getShapeEnemy(enemies[i].code);
			memcpy(enemies[i].object, shape, strlen(shape) + 1);			
		}
	}

}

const char* getShapeEnemy(int enemyCode)
{
	const char* shape = nullptr;

	switch (enemyCode)
	{
	case CODE_ENEMY1:
		shape = SHAPE_ENEMY1;
		break;
	case CODE_ENEMY2:
		shape = SHAPE_ENEMY2;
		break;
	case CODE_ENEMY3:
		shape = SHAPE_ENEMY3;
		break;
	case CODE_ENEMY4:
		shape = SHAPE_ENEMY4;
		break;
	}

	return shape;
}

int getBulletCodeOfEnemy(int enemyCode)
{
	int bulletCode = 0;

	switch (enemyCode)
	{
	case CODE_ENEMY1:
		bulletCode = CODE_ENEMY_BULLET1;
		break;
	case CODE_ENEMY2:
		bulletCode = CODE_ENEMY_BULLET2;
		break;
	case CODE_ENEMY3:
		bulletCode = CODE_ENEMY_BULLET1;
		break;
	case CODE_ENEMY4:
		bulletCode = CODE_ENEMY_BULLET2;
		break;
	}
	return bulletCode;
}

void respawnEnemies(int numOfEnemy, Enemy* enemies)
{
	for (int i = 0; i < numOfEnemy; ++i)
	{
		if (!enemies[i].canRpw || enemies[i].isRpw)
			continue;
		
		if (enemies[i].hp <= 0 || enemies[i].rpwFrame > g_Frames)
			continue;		 

		enemies[i].isRpw	= true;
		enemies[i].mvFrame	= g_Frames;
		enemies[i].atFrame	= g_Frames;
	}
}

void moveEnemies(int numOfEnemy, Enemy* enemies, Player* player)
{
	for (int i = 0; i < numOfEnemy; i++)
	{
		if (enemies[i].iPosY > GAME_SCREEN_HIGHT - 1)
			enemies[i].hp = 0;

		if (enemies[i].hp <= 0 )
			continue;

		if (!enemies[i].isRpw)
			continue;

		int mvFrame = enemies[i].mvFrame + enemies[i].mvDelay;

		if (g_Frames > mvFrame)
		{
			enemies[i].mvFrame = mvFrame;

			int iPosX = enemies[i].iPosX;
			int iPosY = enemies[i].iPosY;

			switch (enemies[i].code)
			{
			case CODE_ENEMY1:				
				iPosY += 1;				
				break;

			case CODE_ENEMY2:
				iPosY += 1;

				if (player->iPosX > iPosX)
					iPosX += 3;
				else if (player->iPosX < iPosX)
					iPosX -= 3;

				break;

			case CODE_ENEMY3:
				if (player->iPosX > iPosX)
					iPosX +=2;
				else if (player->iPosX < iPosX)
					iPosX -=2;
				break;

			case CODE_ENEMY4:
				if (player->iPosX > iPosX)
					iPosX += 3;
				else if (player->iPosX < iPosX)
					iPosX -=3;

				break;
			}

			if (iPosX > 0 && (iPosX + strlen(enemies[i].object)) < GAME_SCREEN_WIDTH - 1)
				enemies[i].iPosX = iPosX;

			if (iPosY > 0)
				enemies[i].iPosY = iPosY;
		}
	}
}
