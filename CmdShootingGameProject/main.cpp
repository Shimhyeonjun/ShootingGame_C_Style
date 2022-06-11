#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <memory.h>
#include <Windows.h>
#include <time.h>

#include "Console.h"
#include "GameLoad.h"
#include "GameDef.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"

void Buffer_Flip(void);
void Buffer_Clear(void);
void Sprite_Draw(int iX, int iY, char chSprite);

/* Game */
void UpdateTitle();
void UpdateLoad();
void UpdateGame();
void UpdateGameEnd();

/* Setting */
void UpdateSetting();
void UpdateStageSetting();
void UpdatePageSetting();
void UpdatePlayerSetting();
void UpdatePlayerRespawnSetting();
void UpdateEnemySetting();

// ------SCENCE ------

/* Game */
#define SCENCE_TITLE	0x01
#define SCENCE_LOAD		0x02
#define SCENCE_GAME		0x03
#define SCENCE_CLEAR	0x04
#define SCENCE_OVER		0x05
#define SCENCE_EXIT		0x06

/* Setting */
#define SCENCE_SETTING					0x10
#define SCENCE_STAGE_SETTING			0x11
#define SCENCE_PAGE_SETTING				0x12
#define SCENCE_ENEMY_RESPAWN_SETTING	0x13
#define SCENCE_PLAYER_SETTING			0x14
#define SCENCE_PLAYER_RESPAWN_SETTING	0x15
#define SCENCE_ENEMY_SETTING			0x16


// --- Setting Entry ---

#define NONE							0x00
#define SETTING_HP						0x21
#define SETTING_RESPWAN					0x22
#define SETTING_MV_PER_FPS				0x23
#define SETTING_ATTACK_PER_FPS			0x24
#define SETTING_BULLET_MV_PER_FPS		0x25
#define SETTING_BULLET_DAMAGE			0x26
#define SETTING_RESPWAN_INTERVAL		0x27
#define SETTING_STAGE_NUMBER			0x28
#define SETTING_STAGE_OPEN				0x29
#define SETTING_PAGE					0x2A
#define SETTING_SCORE					0x2B

struct Menu
{
	int kind;
	int iPosX;
	int iPosY;
	const char* str;
};

struct Cusor
{
	int kind;
	int iPosX;
	int iPosY;
	const char* str;
	bool focus;
};

int g_Scence;
char szScreenBuffer[MAX_SCREEN_HIGHT][MAX_SCREEN_WIDTH];

/* 설정 임시 변수 */ 
SettingInfo sInfoTemp;
Stage stageTemp;

/* 설정 정보 */
SettingInfo sInfo;
EnemyResource eResource;
const char* stageFileName;

/* In Game 정보 */
int	g_Frames;
int g_Stage;	
int g_Page;
int g_ClearPage;
int g_NumOfEnemy;

Stage	stage;
Player	player;

Enemy	enemies[MAX_STAGE_ENEMY];

Bullet	bulletsOfPlayer[SIZE_PLAYER_BULLET];
Bullet	bulletsOfenemy[SIZE_ENEMY_BULLET];

bool visuablitty;

int main(void)
{
	char command[30];

	sprintf(command, "mode con : lines=%d cols=%d", MAX_SCREEN_HIGHT, MAX_SCREEN_WIDTH);
	system(command);

	cs_Initial();

	g_Scence = SCENCE_TITLE;

	while (true)
	{
		if (g_Scence == SCENCE_EXIT)
			break;

		switch (g_Scence)
		{
		/* Game */
		case SCENCE_TITLE:
			UpdateTitle();
			break;

		case SCENCE_LOAD:
			UpdateLoad();
			break;

		case SCENCE_GAME:
			UpdateGame();
			break;
						
		case SCENCE_CLEAR:			
		case SCENCE_OVER:			
			UpdateGameEnd();
			break;		

		/* Setting */
		case SCENCE_SETTING:
			UpdateSetting();
			break;

		case SCENCE_STAGE_SETTING:
			UpdateStageSetting();
			break;

		case SCENCE_PAGE_SETTING:
			UpdatePageSetting();
			break;

		case SCENCE_PLAYER_SETTING:
			UpdatePlayerSetting();
			break;

		case SCENCE_PLAYER_RESPAWN_SETTING:
			UpdatePlayerRespawnSetting();
			break;

		case SCENCE_ENEMY_SETTING:
			UpdateEnemySetting();
			break;
		}

		// 10 프레임
		Sleep(100);
	}

	return 0;
}



//--------------------------------------------------------------------
// 버퍼의 내용을 화면으로 찍어주는 함수.
//
// 적군,아군,총알 등을 szScreenBuffer 에 넣어주고, 
// 1 프레임이 끝나는 마지막에 본 함수를 호출하여 버퍼 -> 화면 으로 그린다.
//--------------------------------------------------------------------
void Buffer_Flip(void)
{
	int iCnt;
	for (iCnt = 0; iCnt < MAX_SCREEN_HIGHT; iCnt++)
	{
		cs_MoveCursor(0, iCnt);
		printf(szScreenBuffer[iCnt]);
	}
}


//--------------------------------------------------------------------
// 화면 버퍼를 지워주는 함수
//
// 매 프레임 그림을 그리기 직전에 버퍼를 지워 준다. 
// 안그러면 이전 프레임의 잔상이 남으니까
//--------------------------------------------------------------------
void Buffer_Clear(void)
{
	int iCnt;
	memset(szScreenBuffer, ' ', MAX_SCREEN_WIDTH * MAX_SCREEN_HIGHT);

	for (iCnt = 0; iCnt < MAX_SCREEN_HIGHT; iCnt++)
	{
		szScreenBuffer[iCnt][MAX_SCREEN_WIDTH - 1] = '\0';
	}

}

//--------------------------------------------------------------------
// 버퍼의 특정 위치에 원하는 문자를 출력.
//
// 입력 받은 X,Y 좌표에 아스키코드 하나를 출력한다. (버퍼에 그림)
//--------------------------------------------------------------------
void Sprite_Draw(int iX, int iY, char chSprite)
{
	if (iX < 0 || iY < 0 || iX >= MAX_SCREEN_WIDTH - 1 || iY >= MAX_SCREEN_HIGHT)
		return;

	szScreenBuffer[iY][iX] = chSprite;
}

void UpdateTitle() 
{
	g_Stage = 0;

	const int TITLE_HIGHT = 11;
	const int NUM_OF_MENU = 3;

	const int TITLE_POSX = 40;
	const int TITLE_POSY = 2;

	const int MENU_POSX = 50;
	const int MENU_POSY = 18;
	const int MENU_POSY_DISTANCE = 2;

	const int CURSOR_POSX_DISTANCE = 4;

	static Cusor cursorMenu = { 0, MENU_POSX - CURSOR_POSX_DISTANCE, MENU_POSY, "=>", true };

	const char* title[TITLE_HIGHT] =
	{
		"#### ####  ##   ###  ###              ",
		"#    #  # #  # #    #                 ",
		"#### #### #### #    ####              ",
		"   # #    #  # #    #                 ",
		"#### #    #  #  ###  ###              ",
		"                                      ",
		"           #####  ##  #### #### #   # ",
		"           # # # #  # #  #  ##  ##  # ",
		"           # # # #### ###   ##  # # # ",
		"           #   # #  # #  #  ##  #  ## ",
		"           #   # #  # #  # #### #   # "
	};

	Menu menu[NUM_OF_MENU] =
	{
		{ SCENCE_LOAD, MENU_POSX, MENU_POSY, "Game Start" },
		{ SCENCE_SETTING, MENU_POSX, MENU_POSY + MENU_POSY_DISTANCE, "Game Setting"},
		{ SCENCE_EXIT, MENU_POSX, MENU_POSY + (MENU_POSY_DISTANCE * 2), "Game exit"}
	};


	if ( (GetAsyncKeyState(VK_UP) & 0x8001) != 0)
	{
		if (cursorMenu.iPosY > MENU_POSY)
			cursorMenu.iPosY = cursorMenu.iPosY - MENU_POSY_DISTANCE;

	}
	else if ( (GetAsyncKeyState(VK_DOWN) & 0x8001) != 0)
	{
		if (cursorMenu.iPosY < MENU_POSY + ((NUM_OF_MENU - 1) * 2))
			cursorMenu.iPosY = cursorMenu.iPosY + MENU_POSY_DISTANCE;
	}
	else if ( (GetAsyncKeyState(VK_SPACE) & 0x8001) != 0)
	{
		for (int i = 0; i < NUM_OF_MENU; i++)
		{
			if (menu[i].iPosY == cursorMenu.iPosY)
				g_Scence = menu[i].kind;
		}
		return;
	}

	else if (((GetAsyncKeyState(VK_LEFT) & 0x8001) != 0)
		|| ((GetAsyncKeyState(VK_RIGHT) & 0x8001) != 0)
		|| (((GetAsyncKeyState(VK_SPACE) & 0x8001) != 0)
		|| ((GetAsyncKeyState(VK_ESCAPE) & 0x8001) != 0)
		|| ((GetAsyncKeyState(VK_TAB) & 0x8001) != 0)
		|| ((GetAsyncKeyState(VK_BACK) & 0x8001) != 0)
		|| ((GetAsyncKeyState(VK_RETURN) & 0x8001) != 0)
		|| ((GetAsyncKeyState(VK_INSERT) & 0x8001) != 0)
		|| ((GetAsyncKeyState(VK_DELETE) & 0x8001) != 0)
		|| ((GetAsyncKeyState(VK_PRIOR) & 0x8001) != 0)
		|| ((GetAsyncKeyState(VK_NEXT) & 0x8001) != 0)))
	{
		// nothing
	}

	Buffer_Clear();
	
	if (cursorMenu.focus)
	{
		if (visuablitty)
		{
			for (int i = 0; i < strlen(cursorMenu.str); ++i)
				Sprite_Draw(cursorMenu.iPosX + i, cursorMenu.iPosY, cursorMenu.str[i]);
		}
		else
		{
			for (int i = 0; i < strlen(cursorMenu.str); ++i)
				Sprite_Draw(cursorMenu.iPosX + i, cursorMenu.iPosY, ' ');
		}

		visuablitty = !visuablitty;
	}

	for (int i = 0; i < TITLE_HIGHT; ++i)
	{
		for (int j = 0; j < strlen(title[i]); ++j)
		{
			Sprite_Draw(TITLE_POSX + j, TITLE_POSY+ i, title[i][j]);
		}
		
	}

	for (int i = 0; i < NUM_OF_MENU; ++i)
	{
		for (int j = 0; j < strlen(menu[i].str); ++j)
		{
			Sprite_Draw(menu[i].iPosX + j, menu[i].iPosY, menu[i].str[j]);
		}		
	}

	Buffer_Flip();	
}

void UpdateLoad()
{	
	if (0 == g_Stage)
	{
		// 설정 파일이 없다면 프로그램을 죽인다.

		memset(&sInfo, 0, sizeof(SettingInfo));		
		loadSettingInfo(&sInfo);
		
		loadResource(&eResource);

		for (int i = 0; i < sInfo.numOfStage; ++i)
		{
			for (int j = 0; j < sInfo.numOfStage - i - 1; ++j)
			{
				if (sInfo.hList[j].order > sInfo.hList[j + 1].order)
				{
					StageHeader temp = sInfo.hList[j + 1];
					sInfo.hList[j + 1] = sInfo.hList[j];
					sInfo.hList[j] = temp;
				}
			}
		}

		memcpy(player.object, SHAPE_PLAYER, strlen(SHAPE_PLAYER) + 1);
		player.hp				= sInfo.pInfo.DefaultHp;
		player.mvDelay			= sInfo.pInfo.DefaultMvDelay;
		player.atDelay			= sInfo.pInfo.DefaultAtDelay;
		
		player.bCode			= CODE_PLAYER_BULLET;
		player.bMvDelay			= sInfo.pInfo.DefaultBulletMvDelay;
		player.bDamege			= sInfo.pInfo.DefaultBulletDamege;
		
		player.state			= STATE_PLAYER_NORMAL;
		player.stateFrame		= 0;
	}

	// ----------------- 아직 사용하진 않음
	player.atFrame = g_Frames;
	player.mvFrame = g_Frames;
	// ------------------

	player.iPosX = sInfo.pInfo.DefaultX;
	player.iPosY = sInfo.pInfo.DefaultY;

	if (g_Stage == sInfo.numOfStage)
	{
		g_Scence = SCENCE_CLEAR;
		return;
	}	 

	while (g_Stage < sInfo.numOfStage)
	{
		if (sInfo.hList[g_Stage].isOpen)
			break;

		++g_Stage;
	}

	g_Frames = 0;
	g_Page = 0;
	g_ClearPage = 0;
	g_NumOfEnemy = 0;

	memset(&stage, 0, sizeof(Stage));
	loadStageInfo(sInfo.hList[g_Stage].szName, &stage);
	
	memset(bulletsOfPlayer, 0, sizeof(Bullet) * SIZE_PLAYER_BULLET);
	memset(bulletsOfenemy, 0, sizeof(Bullet) * SIZE_ENEMY_BULLET);

	memset(enemies, 0, sizeof(Enemy) * MAX_STAGE_ENEMY);

	for (int i = 0; i < MAX_PAGE; ++i)
	{
		for (int j = 0; j < stage.pages[i].numOfEnemy - i - 1; ++j) 
		{
			if (stage.pages[i].rList[j].order > stage.pages[i].rList[j + 1].order)
			{
				EnemyRespawnInfo rwpInfo = stage.pages[i].rList[j + 1];
				stage.pages[i].rList[j + 1] = stage.pages[i].rList[j];
				stage.pages[i].rList[j] = rwpInfo;
			}
		}
	}

	for (int i = 0; i < MAX_PAGE; ++i)
	{
		for (int j = 0; j < stage.pages[i].numOfEnemy; ++j)
		{
			enemies[g_NumOfEnemy].pageNumber = i;
			enemies[g_NumOfEnemy].code = stage.pages[i].rList[j].code;
			enemies[g_NumOfEnemy].order = stage.pages[i].rList[j].order;
			enemies[g_NumOfEnemy].iPosX = stage.pages[i].rList[j].rpwX;
			enemies[g_NumOfEnemy].iPosY = stage.pages[i].rList[j].rpwY;
			++g_NumOfEnemy;
		}
	}

	for (int i = 0 ; i < g_NumOfEnemy; ++i)
	{
		const char* shape = nullptr;

		for (int j = 0; j < sInfo.numOfEnemy; ++j)
		{
			if (enemies[i].code == sInfo.eInfo[j].code)
			{
				shape = getShapeEnemy(sInfo.eInfo[j].code);
				
				memcpy(enemies[i].object, shape, strlen(shape));
				enemies[i].hp		= sInfo.eInfo[j].DefaultHp;
				enemies[i].mvDelay	= sInfo.eInfo[j].DefaultMvDelay;
				enemies[i].atDelay	= sInfo.eInfo[j].DefaultAtDelay;

				enemies[i].bCode	= getBulletCodeOfEnemy(sInfo.eInfo[j].code);
				enemies[i].bMvDelay	= sInfo.eInfo[j].DefaultBulletMvDelay;
				enemies[i].bDemage	= sInfo.eInfo[j].DefaultBulletDamege;

				enemies[i].score = sInfo.eInfo[j].DefaultScore;
				break;
			}

		}
	}

	g_Scence = SCENCE_GAME;
}

void UpdateGame()
{
	if (g_ClearPage == g_Page)
	{
		if (g_ClearPage >= MAX_PAGE)
		{
			++g_Stage;
			g_Scence = SCENCE_LOAD;
			return;
		}

		__int64 beforeOrder = 0;

		int rwpFPS = g_Frames;
		int rwpInterval = sInfo.hList[g_Stage].rpwInterval;
		
		for (int i = 0; i < g_NumOfEnemy; ++i)
		{
			if (enemies[i].pageNumber == g_Page)
			{
				if (enemies[i].order > beforeOrder)
				{
					beforeOrder = enemies[i].order;
					rwpFPS += rwpInterval;
				}
				enemies[i].canRpw	= true;
				enemies[i].rpwFrame = rwpFPS;
			}
		}                                                                                                  

		++g_Page;
	}
	
	++g_Frames;

	if (player.hp <= 0)
	{
		g_Scence = SCENCE_OVER;
		return;
	}

	if ((GetAsyncKeyState(VK_UP) & 0x8001) != 0)
	{
		if (player.iPosY > 0)
			--player.iPosY;
	}
	else if ((GetAsyncKeyState(VK_DOWN) & 0x8001) != 0)
	{
		if (player.iPosY < GAME_SCREEN_HIGHT - 1)
			++player.iPosY;
	}

	else if ((GetAsyncKeyState(VK_LEFT) & 0x8001) != 0)
	{
		if (player.iPosX > 0)
			--player.iPosX;
	}
	else if ((GetAsyncKeyState(VK_RIGHT) & 0x8001) != 0)
	{
		if (player.iPosX < GAME_SCREEN_WIDTH - 1)
			++player.iPosX;
	}

	else if ((GetAsyncKeyState(VK_ESCAPE) & 0x8001) != 0)
	{
		g_Scence = SCENCE_TITLE;	
	}

	else if ( ((GetAsyncKeyState(VK_SPACE) & 0x8001) != 0)
			|| ((GetAsyncKeyState(VK_TAB) & 0x8001) != 0)
			|| ((GetAsyncKeyState(VK_BACK) & 0x8001) != 0)
			|| ((GetAsyncKeyState(VK_RETURN) & 0x8001) != 0)
			|| ((GetAsyncKeyState(VK_INSERT) & 0x8001) != 0)
			|| ((GetAsyncKeyState(VK_DELETE) & 0x8001) != 0)
			|| ((GetAsyncKeyState(VK_PRIOR) & 0x8001) != 0)
			|| ((GetAsyncKeyState(VK_NEXT) & 0x8001) != 0))
	{
		// nothing
	}

	checkPlayer(&player);
	
	checkEnemies(g_NumOfEnemy, enemies);

	moveEnemies(g_NumOfEnemy, enemies, &player);

	respawnEnemies(g_NumOfEnemy, enemies);
	
	moveBulletsOfEnemyObj(SIZE_ENEMY_BULLET, bulletsOfenemy);

	makeBulletOfEnemyObj(SIZE_ENEMY_BULLET, bulletsOfenemy, g_NumOfEnemy, enemies);
	
	moveBulletsOfPlayerObj(SIZE_PLAYER_BULLET, bulletsOfPlayer);

	if ((GetAsyncKeyState(VK_SPACE) & 0x8001) != 0)
	{
		makeBulletOfPlayerObj(SIZE_PLAYER_BULLET, bulletsOfPlayer, &player);
	}
	
	detectCollisonPlayer(SIZE_ENEMY_BULLET, bulletsOfenemy, &player);
	detectCollisonEnemy(SIZE_PLAYER_BULLET, bulletsOfPlayer, g_NumOfEnemy, enemies, &player);

	int deadEnmies = 0;

	for (int i = 0; i < g_NumOfEnemy; ++i)
	{
		if ((enemies[i].pageNumber == g_ClearPage) &&  (enemies[i].hp <= 0))
			++deadEnmies;
	}
	
	if (stage.pages[g_ClearPage].numOfEnemy == deadEnmies)
		g_ClearPage = g_Page;

	Buffer_Clear();

	for (int i = 0; i < GAME_SCREEN_HIGHT; ++i)
	{		
		Sprite_Draw(GAME_SCREEN_WIDTH, i, '|');
	}
	
	if (player.hp > 0)
	{		
		for (int i = 0; i < strlen(player.object); ++i)
		{
			Sprite_Draw(player.iPosX + i, player.iPosY, player.object[i]);
		}
	}

	for (int i = 0; i < g_NumOfEnemy; ++i)
	{
		if (enemies[i].hp <= 0 || !enemies[i].isRpw)
			continue;

		for (int j = 0; j < strlen(enemies[i].object); ++j)
		{
			Sprite_Draw(enemies[i].iPosX + j, enemies[i].iPosY, enemies[i].object[j]);
		}
	}
	
	for (int i = 0; i < SIZE_PLAYER_BULLET; ++i)
	{
		if (!bulletsOfPlayer[i].alive)
			continue;
		
		for (int j = 0; j < strlen(bulletsOfPlayer[i].object); ++j)
		{
			Sprite_Draw(bulletsOfPlayer[i].iPosX + j, bulletsOfPlayer[i].iPosY, bulletsOfPlayer[i].object[j]);
		}
		
	}

	for (int i = 0; i < SIZE_ENEMY_BULLET; ++i)
	{
		if (!bulletsOfenemy[i].alive)
			continue;
		
		for (int j = 0; j < strlen(bulletsOfenemy[i].object); ++j)
		{
			Sprite_Draw(bulletsOfenemy[i].iPosX + j, bulletsOfenemy[i].iPosY, bulletsOfenemy[i].object[j]);
		}
		
	}
	const int NUM_OF_DESCRIPTION = 3;

	Menu description[NUM_OF_DESCRIPTION]
	{
		{0, 90, 20, "Key Operation <K>"},
		{0, 90, 23, "ATTACK : <SPACE>"},
		{0, 90, 25, "ESCAPE : <ESC>"}
	};

	for (int i = 0; i < NUM_OF_DESCRIPTION; ++i)
	{
		for (int j = 0; j < strlen(description[i].str); ++j)
		{
			Sprite_Draw(description[i].iPosX + j, description[i].iPosY, description[i].str[j]);
		}		
	}

	const int szBufSize = 30;
	char buf[szBufSize];

	snprintf(buf, szBufSize, "%s : %d", "Stage : ", g_Stage + 1);

	for (int i = 0; i < strlen(buf); ++i)
	{
		Sprite_Draw(90 + i, 10, buf[i]);
	}

	snprintf(buf, szBufSize, "%s : %d", "HP : ", player.hp);

	for (int i = 0; i < strlen(buf); ++i)
	{
		Sprite_Draw(90 + i, 12, buf[i]);
	}	

	snprintf(buf, szBufSize, "%s : %d", "SCORE : ", player.totalScore);

	for (int i = 0; i < strlen(buf); ++i)
	{
		Sprite_Draw(90 + i, 14, buf[i]);
	}

	Buffer_Flip();
}

void UpdateGameEnd()
{
	const int ANNOUNCE_POSX = 30;
	const int ANNOUNCE_POSY = 5;

	const int SIZE_ANNOUNCE = 11;
	const char* gameOver[SIZE_ANNOUNCE] =
	{
		"####  ##  ##### ####      ",
		"#    #  # # # # #         ",
		"# ## #### # # # ####      ",
		"#  # #  # #   # #         ",
		"#### #  # #   # ####      ",
		"                          ",
		"     #### #  # #### ####  ",
		"     #  # #  # #    #  #  ",
		"     #  # #  # #### ###   ",
		"     #  # #  # #    #  #  ",
		"     ####  ##  #### #  #  "
	};

	const char* gameClear[SIZE_ANNOUNCE] =
	{
		"####  ##  ##### ####           ",
		"#    #  # # # # #              ",
		"# ## #### # # # ####           ",
		"#  # #  # #   # #              ",
		"#### #  # #   # ####           ",
		"                               ",
		"      ### #     ##  #### ####  ",
		"     #    #    #  # #    #  #  ",
		"     #    #    #### #### ###   ",
		"     #    #    #  # #    #  #  ",
		"      ### #### #  # #### #  #  "
	};

	if ((GetAsyncKeyState(VK_ESCAPE) & 0x8001) != 0)
	{
		g_Scence = SCENCE_TITLE;
	}

	else if (((GetAsyncKeyState(VK_UP) & 0x8001) != 0)
		|| ((GetAsyncKeyState(VK_DOWN) & 0x8001) != 0)
		|| ((GetAsyncKeyState(VK_LEFT) & 0x8001) != 0)
		|| ((GetAsyncKeyState(VK_RIGHT) & 0x8001) != 0)
		|| (((GetAsyncKeyState(VK_SPACE) & 0x8001) != 0)
			|| ((GetAsyncKeyState(VK_ESCAPE) & 0x8001) != 0)
			|| ((GetAsyncKeyState(VK_TAB) & 0x8001) != 0)
			|| ((GetAsyncKeyState(VK_BACK) & 0x8001) != 0)
			|| ((GetAsyncKeyState(VK_RETURN) & 0x8001) != 0)
			|| ((GetAsyncKeyState(VK_INSERT) & 0x8001) != 0)
			|| ((GetAsyncKeyState(VK_DELETE) & 0x8001) != 0)
			|| ((GetAsyncKeyState(VK_PRIOR) & 0x8001) != 0)
			|| ((GetAsyncKeyState(VK_NEXT) & 0x8001) != 0)))
	{
		// nothing
	}

	Buffer_Clear();

	for (int i = 0; i < GAME_SCREEN_HIGHT; ++i)
	{
		Sprite_Draw(GAME_SCREEN_WIDTH, i, '|');
	}

	if (g_Scence == SCENCE_CLEAR)
	{
		for (int i = 0; i < SIZE_ANNOUNCE; ++i)
		{
			for (int j = 0; j < strlen(gameClear[i]); ++j)
			{
				if(visuablitty)
					Sprite_Draw(ANNOUNCE_POSX + j, ANNOUNCE_POSY + i, gameClear[i][j]);
				else
					Sprite_Draw(ANNOUNCE_POSX + j, ANNOUNCE_POSY + i, ' ');								
			}
		}
	}
	else
	{
		for (int i = 0; i < SIZE_ANNOUNCE; ++i)
		{
			for (int j = 0; j < strlen(gameOver[i]); ++j)
			{
				if(visuablitty)
					Sprite_Draw(ANNOUNCE_POSX + j, ANNOUNCE_POSY + i, gameOver[i][j]);
				else
					Sprite_Draw(ANNOUNCE_POSX + j, ANNOUNCE_POSY + i, ' ');				
			}
		}
	}
	
	visuablitty = !visuablitty;

	const int NUM_OF_DESCRIPTION = 2;

	Menu description[NUM_OF_DESCRIPTION]
	{
		{0, 90, 20, "Key Operation <K>"},
		{0, 90, 22, "ESCAPE : <ESC>"}
	};

	for (int i = 0; i < NUM_OF_DESCRIPTION; ++i)
	{
		for (int j = 0; j < strlen(description[i].str); ++j)
		{
			Sprite_Draw(description[i].iPosX + j, description[i].iPosY, description[i].str[j]);
		}
	}

	const int szBufSize = 30;
	char buf[szBufSize];

	snprintf(buf, szBufSize, "%s : %d", "SCORE : ", player.totalScore);

	for (int i = 0; i < strlen(buf); ++i)
	{
		Sprite_Draw(90 + i, 14, buf[i]);
	}

	Buffer_Flip();
}

void UpdateSetting()
{
	const int MENU_POSX = 10;
	const int MENU_POSY = 10;
	const int MENU_POSY_DISTANCE = 4;

	const int CURSOR_POSX_DISTANCE = 4;
	const int NUM_OF_MENU = 3;

	static Cusor cursorMenu = { 0, MENU_POSX - CURSOR_POSX_DISTANCE, MENU_POSY, "=>", true };

	Menu menu[NUM_OF_MENU] =
	{
		{ SCENCE_STAGE_SETTING, MENU_POSX, MENU_POSX,							"Set each stage setting information" },
		{ SCENCE_PLAYER_SETTING,MENU_POSX, MENU_POSX + MENU_POSY_DISTANCE,		"Set player setting information"},
		{ SCENCE_ENEMY_SETTING, MENU_POSX, MENU_POSX + (MENU_POSY_DISTANCE * 2) , "Set enemy setting information"}
	};

	if ((GetAsyncKeyState(VK_UP) & 0x8001) != 0)
	{
		if (cursorMenu.iPosY > MENU_POSY)
			cursorMenu.iPosY -= MENU_POSY_DISTANCE;
	}
	else if ((GetAsyncKeyState(VK_DOWN) & 0x8001) != 0)
	{
		if (cursorMenu.iPosY < MENU_POSY + (MENU_POSY_DISTANCE * (NUM_OF_MENU - 1)))
			cursorMenu.iPosY += MENU_POSY_DISTANCE;
	}

	else if ((GetAsyncKeyState(VK_LEFT) & 0x8001) != 0)
	{
		//nothing
	}

	else if ((GetAsyncKeyState(VK_RIGHT) & 0x8001) != 0)
	{
		//nothing
	}

	else if ((GetAsyncKeyState(VK_SPACE) & 0x8001) != 0)
	{
		for (int i = 0; i < NUM_OF_MENU; ++i)
		{
			if ((menu[i].iPosX == cursorMenu.iPosX + CURSOR_POSX_DISTANCE) && (menu[i].iPosY == cursorMenu.iPosY))
			{
				// 설정 파일이 없다면 프로그램을 죽인다.
				loadSettingInfo(&sInfo);

				loadResource(&eResource);

				memset(&sInfoTemp, 0, sizeof(SettingInfo));

				sInfoTemp.numOfStage = sInfo.numOfStage;
				sInfoTemp.pInfo = sInfo.pInfo;
				memcpy(&sInfoTemp.hList, &sInfo.hList, sizeof(sInfo.hList));				
				
				for (int i = 0; i < eResource.numOfResource; ++i)
				{
					sInfoTemp.eInfo[sInfoTemp.numOfEnemy++].code = eResource.code[i];
				}

				for (int i = 0; i < sInfoTemp.numOfEnemy; ++i)
				{
					for (int j = 0; j < sInfo.numOfEnemy; ++j)
					{
						if (sInfoTemp.eInfo[i].code == sInfo.eInfo[j].code)
						{
							sInfoTemp.eInfo[i] = sInfo.eInfo[j];
							break;
						}
					}
				}

				g_Scence = menu[i].kind;
			}			
		}
	}

	else if ((GetAsyncKeyState(VK_ESCAPE) & 0x8001) != 0)
	{
		g_Scence = SCENCE_TITLE;
		cursorMenu.iPosX = MENU_POSX - CURSOR_POSX_DISTANCE;
		cursorMenu.iPosY = MENU_POSY;
	}

	else if ((GetAsyncKeyState(VK_TAB) & 0x8001) != 0)
	{
		//nothing
	}

	else if ((GetAsyncKeyState(VK_BACK) & 0x8001) != 0)
	{
		//nothing
	}

	else if ((GetAsyncKeyState(VK_RETURN) & 0x8001) != 0)
	{
		//nothing
	}

	else if ((GetAsyncKeyState(VK_INSERT) & 0x8001) != 0)
	{
		//nothing
	}

	else if ((GetAsyncKeyState(VK_DELETE) & 0x8001) != 0)
	{
		//nothing
	}

	else if ((GetAsyncKeyState(VK_PRIOR) & 0x8001) != 0)
	{
		//nothing
	}

	else if ((GetAsyncKeyState(VK_NEXT) & 0x8001) != 0)
	{
		//nothing
	}

	Buffer_Clear();

	if (cursorMenu.focus)
	{
		if (visuablitty)
		{
			for (int i = 0; i < strlen(cursorMenu.str); ++i)
				Sprite_Draw(cursorMenu.iPosX + i, cursorMenu.iPosY, cursorMenu.str[i]);
		}
		else
		{
			for (int i = 0; i < strlen(cursorMenu.str); ++i)
				Sprite_Draw(cursorMenu.iPosX + i, cursorMenu.iPosY, ' ');
		}

		visuablitty = !visuablitty;
	}

	for (int i = 0; i < NUM_OF_MENU; i++)
	{
		for (int j = 0; j < strlen(menu[i].str); ++j)
		{
			Sprite_Draw(menu[i].iPosX + j, menu[i].iPosY, menu[i].str[j]);
		}
	}

	Buffer_Flip();
}


/*
	유저에서 Move Delay, Attack Delay는 실제 게임 로직에 적용 X
	혹시 나중에 사용할까 싶어서
*/

void UpdatePlayerSetting()
{
	const int MENU_POSX = 70;
	const int MENU_POSY = 5;
	const int MENU_POSY_DISTANCE = 2;

	const int CURSOR_POSX_DISTANCE = 4;
	
	const int NUM_OF_MENU = 6;
	const int NUM_OF_DESCRIPTION = 6;

	static Cusor cursorMenu = { 0, MENU_POSX - CURSOR_POSX_DISTANCE, MENU_POSY, "=>", true };

	static Cusor cursorUp = { 0, 105, 0, "->" , false };
	static Cusor cursorDown = { 0, 100, 0, "<-" , false };

	Menu menu[NUM_OF_MENU] =
	{
		{SETTING_HP, MENU_POSX, MENU_POSY,											"                 HP : "},
		{SETTING_MV_PER_FPS, MENU_POSX, MENU_POSY + MENU_POSY_DISTANCE,				"         Move Delay : "},
		{SETTING_ATTACK_PER_FPS, MENU_POSX, MENU_POSY + MENU_POSY_DISTANCE,	"       Attack Delay : "},
		{SETTING_BULLET_MV_PER_FPS, MENU_POSX, MENU_POSY + (MENU_POSY_DISTANCE * 2),"  Bullet Move Delay : "},
		{SETTING_BULLET_DAMAGE, MENU_POSX, MENU_POSY + (MENU_POSY_DISTANCE * 3),	"      Bullet Damage : "},
		{SETTING_RESPWAN, MENU_POSX, MENU_POSY + (MENU_POSY_DISTANCE * 4),			"   Respawn Position : "}
	};

	Menu description[NUM_OF_DESCRIPTION]
	{
		{0, 25, MENU_POSY + 5, "Player Object"},
		{0, 10, 20, "Key Operation <K>"},
		{0, 20, 23, "SELECT : <SPACE>"},
		{0, 50, 23, "ESCAPE : <ESC>"},
		{0, 80, 23, "SAVE : <ENTER>"},
		{0, 20, 26, "BACK : <BACK SPACE>"}
	};

	if ((GetAsyncKeyState(VK_UP) & 0x8001) != 0)
	{
		if (cursorMenu.focus)
		{
			if (cursorMenu.iPosY > MENU_POSY)
				cursorMenu.iPosY = cursorMenu.iPosY - MENU_POSY_DISTANCE;
		}
		
	}
	else if ((GetAsyncKeyState(VK_DOWN) & 0x8001) != 0)
	{
		if (cursorMenu.focus)
		{
			if (cursorMenu.iPosY < MENU_POSY + (MENU_POSY_DISTANCE * (NUM_OF_MENU - 1)))
				cursorMenu.iPosY = cursorMenu.iPosY + MENU_POSY_DISTANCE;
		}		
	}
	else if ((GetAsyncKeyState(VK_LEFT) & 0x8001) != 0)
	{
		if (cursorUp.focus)
		{
			cursorUp.focus	= false;
			cursorDown.focus = true;
		}

	}
	else if ((GetAsyncKeyState(VK_RIGHT) & 0x8001) != 0)
	{
		if (cursorDown.focus)
		{
			cursorUp.focus = true;
			cursorDown.focus = false;
		}
	}
	else if ((GetAsyncKeyState(VK_SPACE) & 0x8001) != 0)
	{
		if (cursorMenu.focus)
		{
			for (int i = 0; i < NUM_OF_MENU; i++)
			{
				if (menu[i].iPosX == cursorMenu.iPosX + CURSOR_POSX_DISTANCE && menu[i].iPosY == cursorMenu.iPosY)
				{
					if (SETTING_RESPWAN == menu[i].kind)
					{
						g_Scence = SCENCE_PLAYER_RESPAWN_SETTING;
						break;
					}

					cursorMenu.focus = false;

					cursorUp.kind = menu[i].kind;
					cursorDown.kind = menu[i].kind;

					cursorUp.iPosY	= menu[i].iPosY;
					cursorDown.iPosY = menu[i].iPosY;
					
					cursorUp.focus = true;
					cursorDown.focus = false;
				}
			}
		}

		else if (cursorUp.focus)
		{			
			switch (cursorUp.kind)
			{
			case SETTING_HP:
				++sInfoTemp.pInfo.DefaultHp;
				break;
			
			case SETTING_MV_PER_FPS:
				++sInfoTemp.pInfo.DefaultMvDelay;
				break;
			
			case SETTING_ATTACK_PER_FPS:
				++sInfoTemp.pInfo.DefaultAtDelay;
				break;

			case SETTING_BULLET_MV_PER_FPS:
				++sInfoTemp.pInfo.DefaultBulletMvDelay;
				break;

			case SETTING_BULLET_DAMAGE:
				++sInfoTemp.pInfo.DefaultBulletDamege;
				break;
			}
		}
		else if (cursorDown.focus)
		{
			switch (cursorDown.kind)
			{
			case SETTING_HP:
				if (sInfoTemp.pInfo.DefaultHp > 0)
					--sInfoTemp.pInfo.DefaultHp;
				break;
			
			case SETTING_MV_PER_FPS:
				if (sInfoTemp.pInfo.DefaultMvDelay > 0)
					--sInfoTemp.pInfo.DefaultMvDelay;
				break;
			
			case SETTING_ATTACK_PER_FPS:
				if (sInfoTemp.pInfo.DefaultAtDelay > 0)
					--sInfoTemp.pInfo.DefaultAtDelay;
				break;

			case SETTING_BULLET_MV_PER_FPS:
				if(sInfoTemp.pInfo.DefaultBulletMvDelay > 0)
					--sInfoTemp.pInfo.DefaultBulletMvDelay;
				break;

			case SETTING_BULLET_DAMAGE:
				if (sInfoTemp.pInfo.DefaultBulletDamege > 0)
					--sInfoTemp.pInfo.DefaultBulletDamege;
				break;
			}
		
		}
	}

	else if ((GetAsyncKeyState(VK_ESCAPE) & 0x8001) != 0)
	{
		cursorMenu.iPosX = MENU_POSX - CURSOR_POSX_DISTANCE;
		cursorMenu.iPosY = MENU_POSY;
		g_Scence = SCENCE_SETTING;

		cursorMenu.focus = true;
		cursorUp.focus = false;
		cursorDown.focus = false;
	}

	else if ((GetAsyncKeyState(VK_TAB) & 0x8001) != 0)
	{
		//nothing	
	}

	else if (((GetAsyncKeyState(VK_BACK) & 0x8001) != 0))
	{
		cursorMenu.focus = true;
		cursorUp.focus = false;
		cursorDown.focus = false;
	}

	else if ((GetAsyncKeyState(VK_RETURN) & 0x8001) != 0)
	{		
		if (!saveSettingInfo(&sInfoTemp))
			return;

		cursorMenu.iPosX = MENU_POSX - CURSOR_POSX_DISTANCE;
		cursorMenu.iPosY = MENU_POSY;
		g_Scence = SCENCE_SETTING;

		cursorMenu.focus = true;
		cursorUp.focus = false;
		cursorDown.focus = false;
	}

	else if ((GetAsyncKeyState(VK_INSERT) & 0x8001) != 0)
	{
		//nothing	
	}

	else if ((GetAsyncKeyState(VK_DELETE) & 0x8001) != 0)
	{
		//nothing	
	}

	else if ((GetAsyncKeyState(VK_PRIOR) & 0x8001) != 0)
	{
		//nothing	
	}

	else if ((GetAsyncKeyState(VK_NEXT) & 0x8001) != 0)
	{
		//nothing	
	}

	Buffer_Clear();

	char buf[30];

	for (int i = 0; i < NUM_OF_MENU; i++)
	{
		int iPosX = menu[i].iPosX;
		int iPosY = menu[i].iPosY;

		switch (menu[i].kind)
		{
		case SETTING_HP:
			snprintf(buf, 30, "%s%3d", menu[i].str, sInfoTemp.pInfo.DefaultHp);
			break;
		
		case SETTING_MV_PER_FPS:
			snprintf(buf, 30, "%s%3d", menu[i].str, sInfoTemp.pInfo.DefaultMvDelay);
			break;
		
		case SETTING_ATTACK_PER_FPS:
			snprintf(buf, 30, "%s%3d", menu[i].str, sInfoTemp.pInfo.DefaultAtDelay);
			break;
		case SETTING_BULLET_MV_PER_FPS:
			snprintf(buf, 30, "%s%3d", menu[i].str, sInfoTemp.pInfo.DefaultBulletMvDelay);
			break;
		case SETTING_BULLET_DAMAGE:
			snprintf(buf, 30, "%s%3d", menu[i].str, sInfoTemp.pInfo.DefaultBulletDamege);
			break;
		case SETTING_RESPWAN:
			snprintf(buf, 30, "%s(%d,%d)", menu[i].str, sInfoTemp.pInfo.DefaultX, sInfoTemp.pInfo.DefaultY);
		}

		for (int j = 0; j < strlen(buf); ++j)
			Sprite_Draw(iPosX + j, iPosY, buf[j]);
	}

	for (int i = 0; i < NUM_OF_DESCRIPTION; ++i)
	{
		for (int j = 0; j < strlen(description[i].str); ++j)
			Sprite_Draw(description[i].iPosX + j, description[i].iPosY, description[i].str[j]);
	}


	if (cursorMenu.focus)
	{
		if (visuablitty)
		{
			for (int i = 0; i < strlen(cursorMenu.str); ++i)
				Sprite_Draw(cursorMenu.iPosX + i, cursorMenu.iPosY, cursorMenu.str[i]);
		}
		else
		{
			for (int i = 0; i < strlen(cursorMenu.str); ++i)
				Sprite_Draw(cursorMenu.iPosX + i, cursorMenu.iPosY, ' ');
		}

		visuablitty = !visuablitty;
	}
	else
	{
		for (int i = 0; i < strlen(cursorMenu.str); ++i)
			Sprite_Draw(cursorMenu.iPosX + i, cursorMenu.iPosY, cursorMenu.str[i]);

		for (int i = 0; i < strlen(cursorUp.str); ++i)
			Sprite_Draw(cursorUp.iPosX + i, cursorUp.iPosY, cursorUp.str[i]);

		for (int i = 0; i < strlen(cursorDown.str); ++i)
			Sprite_Draw(cursorDown.iPosX + i, cursorDown.iPosY, cursorDown.str[i]);

		if (cursorUp.focus)
		{
			if (!visuablitty)
			{
				for (int i = 0; i < strlen(cursorUp.str); ++i)
					Sprite_Draw(cursorUp.iPosX + i, cursorUp.iPosY, ' ');
			}
			visuablitty = !visuablitty;
		}

		if (cursorDown.focus)
		{
			if (visuablitty)
			{
				for (int i = 0; i < strlen(cursorDown.str); ++i)
					Sprite_Draw(cursorDown.iPosX + i, cursorDown.iPosY, ' ');
			}

			visuablitty = !visuablitty;
		}
	}

	for (int i = 0; i < strlen(SHAPE_PLAYER); ++i)
	{
		Sprite_Draw(i + 28, MENU_POSY + 3, SHAPE_PLAYER[i]);
	}

	Buffer_Flip();
}


void UpdatePlayerRespawnSetting()
{
	static Cusor cusorRwp = { 0, sInfoTemp.pInfo.DefaultX + 1, sInfoTemp.pInfo.DefaultY + 1, "P", true };

	const int NUM_OF_DESCRIPTION = 3;

	Menu description[NUM_OF_DESCRIPTION]
	{
		{0, 90, 20, "Key Operation <K>"},
		{0, 90, 22, "SELECT : <SPACE>"},		
		{0, 90, 24, "BACK : <BACK SPACE>"}
	};

	static bool visibility = true;

	if ((GetAsyncKeyState(VK_UP) & 0x8001) != 0)
	{
		if (cusorRwp.iPosY > 0)
			--cusorRwp.iPosY;
	}
	else if ((GetAsyncKeyState(VK_DOWN) & 0x8001) != 0)
	{
		if (cusorRwp.iPosY < GAME_SCREEN_HIGHT - 1)
			++cusorRwp.iPosY;
	}
	else if ((GetAsyncKeyState(VK_LEFT) & 0x8001) != 0)
	{
		if (cusorRwp.iPosX > 0)
			--cusorRwp.iPosX;
	}
	else if ((GetAsyncKeyState(VK_RIGHT) & 0x8001) != 0)
	{
		if (cusorRwp.iPosX < GAME_SCREEN_WIDTH - 1)
			++cusorRwp.iPosX;
	}
	else if ((GetAsyncKeyState(VK_SPACE) & 0x8001) != 0)
	{
		sInfoTemp.pInfo.DefaultX = cusorRwp.iPosX;
		sInfoTemp.pInfo.DefaultY = cusorRwp.iPosY;
	}

	else if ((GetAsyncKeyState(VK_ESCAPE) & 0x8001) != 0)
	{
		// nothing
	}

	else if ((GetAsyncKeyState(VK_TAB) & 0x8001) != 0)
	{
		// nothing
	}	

	else if ((GetAsyncKeyState(VK_BACK) & 0x8001) != 0)
	{
		g_Scence = SCENCE_PLAYER_SETTING;
	}

	else if ((GetAsyncKeyState(VK_RETURN) & 0x8001) != 0)
	{
		// nothing
	}

	else if ((GetAsyncKeyState(VK_INSERT) & 0x8001) != 0)
	{
		// nothing
	}

	else if ((GetAsyncKeyState(VK_DELETE) & 0x8001) != 0)
	{
		// nothing
	}

	else if ((GetAsyncKeyState(VK_PRIOR) & 0x8001) != 0)
	{
		// nothing
	}

	else if ((GetAsyncKeyState(VK_NEXT) & 0x8001) != 0)
	{
		// nothing
	}

	Buffer_Clear();

	for (int i = 0; i < GAME_SCREEN_HIGHT; ++i)
	{
		Sprite_Draw(0, i, '|');
		Sprite_Draw(GAME_SCREEN_WIDTH - 2, i, '|');
	}

	for (int i = 0; i < strlen(SHAPE_PLAYER); ++i)
	{
		Sprite_Draw(sInfoTemp.pInfo.DefaultX + i, sInfoTemp.pInfo.DefaultY, SHAPE_PLAYER[i]);
	}

	for (int i = 0; i < strlen(cusorRwp.str); ++i)
	{
		Sprite_Draw(cusorRwp.iPosX + i, cusorRwp.iPosY, cusorRwp.str[i]);
	}

	if (cusorRwp.focus)
	{
		if (visibility)
			Sprite_Draw(cusorRwp.iPosX, cusorRwp.iPosY, ' ');

		visibility = !visibility;
	}

	for (int i = 0; i < NUM_OF_DESCRIPTION; ++i)
	{
		for (int j = 0; j < strlen(description[i].str); ++j)
			Sprite_Draw(description[i].iPosX + j, description[i].iPosY, description[i].str[j]);
	}


	Buffer_Flip();
}

void UpdateEnemySetting()
{
	const int MENU_POSX = 70;
	const int MENU_POSY = 5;
	const int MENU_POSY_DISTANCE = 2;

	const int CURSOR_POSX_DISTANCE = 4;

	const int NUM_OF_MENU = 6;
	const int NUM_OF_DESCRIPTION = 7;

	static Cusor cursorMenu = { 0, MENU_POSX - CURSOR_POSX_DISTANCE, MENU_POSY, "=>", true };

	static Cusor cursorUp = { 0, 105, 0, "->" , false };
	static Cusor cursorDown = { 0, 100, 0, "<-" , false };

	static int eIndex = 0;

	Menu menu[NUM_OF_MENU] =
	{
		{SETTING_HP, MENU_POSX, MENU_POSY,											"                   HP : "},
		{SETTING_MV_PER_FPS, MENU_POSX, MENU_POSY + MENU_POSY_DISTANCE,				"           Move Delay : "},
		{SETTING_ATTACK_PER_FPS, MENU_POSX, MENU_POSY + (MENU_POSY_DISTANCE * 2),	"         Attack Delay : "},
		{SETTING_BULLET_MV_PER_FPS, MENU_POSX, MENU_POSY + (MENU_POSY_DISTANCE * 3),"    Bullet Move Delay : "},
		{SETTING_BULLET_DAMAGE, MENU_POSX, MENU_POSY + (MENU_POSY_DISTANCE * 4),	"        Bullet Damage : "},
		{SETTING_SCORE, MENU_POSX, MENU_POSY + (MENU_POSY_DISTANCE * 5),	"                SCORE : "}
	};

	Menu description[NUM_OF_DESCRIPTION]
	{
		{0, 25, MENU_POSY + 5, "Enemy Object"},
		{0, 10, 20, "Key Operation <K>"},
		{0, 20, 23, "SELECT : <SPACE>"},
		{0, 50, 23, "SAVE : <ENTER>"},  
		{0, 80, 23, "ESCAPE : <ESC>"},
		{0, 20, 25, "OBJECT : <TAB>"},
		{0, 50, 25, "BACK : <BACK SPACE>"}
	};

	if ((GetAsyncKeyState(VK_UP) & 0x8001) != 0)
	{
		if (cursorMenu.focus)
		{
			if (cursorMenu.iPosY > MENU_POSY)
				cursorMenu.iPosY = cursorMenu.iPosY - MENU_POSY_DISTANCE;
		}

	}
	else if ((GetAsyncKeyState(VK_DOWN) & 0x8001) != 0)
	{
		if (cursorMenu.focus)
		{
			if (cursorMenu.iPosY < MENU_POSY + (MENU_POSY_DISTANCE * (NUM_OF_MENU - 1)))
				cursorMenu.iPosY = cursorMenu.iPosY + MENU_POSY_DISTANCE;
		}
	}
	else if ((GetAsyncKeyState(VK_LEFT) & 0x8001) != 0)
	{
		if (cursorUp.focus)
		{
			cursorUp.focus = false;
			cursorDown.focus = true;
		}

	}
	else if ((GetAsyncKeyState(VK_RIGHT) & 0x8001) != 0)
	{
		if (cursorDown.focus)
		{
			cursorUp.focus = true;
			cursorDown.focus = false;
		}
	}
	else if ((GetAsyncKeyState(VK_SPACE) & 0x8001) != 0)
	{
		if (cursorMenu.focus)
		{
			for (int i = 0; i < NUM_OF_MENU; i++)
			{
				if (menu[i].iPosX == cursorMenu.iPosX + CURSOR_POSX_DISTANCE && menu[i].iPosY == cursorMenu.iPosY)
				{
					cursorMenu.focus = false;

					cursorUp.kind = menu[i].kind;
					cursorDown.kind = menu[i].kind;

					cursorUp.iPosY = menu[i].iPosY;
					cursorDown.iPosY = menu[i].iPosY;

					cursorUp.focus = true;
					cursorDown.focus = false;
				}
			}
		}

		else if (cursorUp.focus)
		{
			switch (cursorUp.kind)
			{
			case SETTING_HP:
				++sInfoTemp.eInfo[eIndex].DefaultHp;
				break;

			case SETTING_MV_PER_FPS:
				++sInfoTemp.eInfo[eIndex].DefaultMvDelay;
				break;

			case SETTING_ATTACK_PER_FPS:
				++sInfoTemp.eInfo[eIndex].DefaultAtDelay;
				break;

			case SETTING_BULLET_MV_PER_FPS:
				++sInfoTemp.eInfo[eIndex].DefaultBulletMvDelay;
				break;

			case SETTING_BULLET_DAMAGE:
				++sInfoTemp.eInfo[eIndex].DefaultBulletDamege;
				break;

			case SETTING_SCORE:
				++sInfoTemp.eInfo[eIndex].DefaultScore;
				break;
			}
		}
		else if (cursorDown.focus)
		{
			switch (cursorDown.kind)
			{
			case SETTING_HP:
				if (sInfoTemp.eInfo[eIndex].DefaultHp > 0)
					--sInfoTemp.eInfo[eIndex].DefaultHp;
				break;

			case SETTING_MV_PER_FPS:
				if (sInfoTemp.eInfo[eIndex].DefaultMvDelay > 0)
					--sInfoTemp.eInfo[eIndex].DefaultMvDelay;
				break;
			case SETTING_ATTACK_PER_FPS:
				if (sInfoTemp.eInfo[eIndex].DefaultAtDelay > 0)
					--sInfoTemp.eInfo[eIndex].DefaultAtDelay;
				break;

			case SETTING_BULLET_MV_PER_FPS:
				if (sInfoTemp.eInfo[eIndex].DefaultBulletMvDelay > 0)
					--sInfoTemp.eInfo[eIndex].DefaultBulletMvDelay;
				break;

			case SETTING_BULLET_DAMAGE:
				if (sInfoTemp.eInfo[eIndex].DefaultBulletDamege > 0)
					--sInfoTemp.eInfo[eIndex].DefaultBulletDamege;
				break;

			case SETTING_SCORE:
				if(sInfoTemp.eInfo[eIndex].DefaultScore > 0)
					--sInfoTemp.eInfo[eIndex].DefaultScore;
				break;
			}
		}
	}

	else if ((GetAsyncKeyState(VK_ESCAPE) & 0x8001) != 0)
	{
		g_Scence = SCENCE_SETTING;
		cursorMenu.iPosX = MENU_POSX - CURSOR_POSX_DISTANCE;
		cursorMenu.iPosY = MENU_POSY;	

		cursorMenu.focus = true;
		cursorUp.focus = false;
		cursorDown.focus = false;

		eIndex = 0;
	}

	else if ((GetAsyncKeyState(VK_TAB) & 0x8001) != 0)
	{
		eIndex = ++eIndex % sInfoTemp.numOfEnemy;
	}

	else if ((GetAsyncKeyState(VK_BACK) & 0x8001) != 0)
	{
		cursorMenu.focus = true;
		cursorUp.focus = false;
		cursorDown.focus = false;
	}

	else if ((GetAsyncKeyState(VK_RETURN) & 0x8001) != 0)
	{
		if (!saveSettingInfo(&sInfoTemp))
			return;

		g_Scence = SCENCE_SETTING;

		cursorMenu.iPosX = MENU_POSX - CURSOR_POSX_DISTANCE;
		cursorMenu.iPosY = MENU_POSY;

		cursorMenu.focus = true;
		cursorUp.focus = false;
		cursorDown.focus = false;

		eIndex = 0;
	}

	else if ((GetAsyncKeyState(VK_INSERT) & 0x8001) != 0)
	{
		// nothing
	}

	else if ((GetAsyncKeyState(VK_DELETE) & 0x8001) != 0)
	{
		// nothing
	}


	else if ((GetAsyncKeyState(VK_PRIOR) & 0x8001) != 0)
	{
		// nothing
	}

	else if ((GetAsyncKeyState(VK_NEXT) & 0x8001) != 0)
	{
		// nothing
	}

	Buffer_Clear();

	char buf[30];

	for (int i = 0; i < NUM_OF_MENU; i++)
	{
		int iPosX = menu[i].iPosX;
		int iPosY = menu[i].iPosY;

		switch (menu[i].kind)
		{
		case SETTING_HP:
			snprintf(buf, 30, "%s%3d", menu[i].str, sInfoTemp.eInfo[eIndex].DefaultHp);
			break;
		case SETTING_MV_PER_FPS:
			snprintf(buf, 30, "%s%3d", menu[i].str, sInfoTemp.eInfo[eIndex].DefaultMvDelay);
			break;
		case SETTING_ATTACK_PER_FPS:
			snprintf(buf, 30, "%s%3d", menu[i].str, sInfoTemp.eInfo[eIndex].DefaultAtDelay);
			break;
		case SETTING_BULLET_MV_PER_FPS:
			snprintf(buf, 30, "%s%3d", menu[i].str, sInfoTemp.eInfo[eIndex].DefaultBulletMvDelay);
			break;
		case SETTING_BULLET_DAMAGE:
			snprintf(buf, 30, "%s%3d", menu[i].str, sInfoTemp.eInfo[eIndex].DefaultBulletDamege);
			break;
		case SETTING_SCORE:
			snprintf(buf, 30, "%s%3d", menu[i].str, sInfoTemp.eInfo[eIndex].DefaultScore);
			break;
		}

		for (int j = 0; j < strlen(buf); ++j)
			Sprite_Draw(iPosX + j, iPosY, buf[j]);
	}

	for (int i = 0; i < NUM_OF_DESCRIPTION; ++i)
	{
		for (int j = 0; j < strlen(description[i].str); ++j)
			Sprite_Draw(description[i].iPosX + j, description[i].iPosY, description[i].str[j]);
	}


	if (cursorMenu.focus)
	{
		if (visuablitty)
		{
			for (int i = 0; i < strlen(cursorMenu.str); ++i)
				Sprite_Draw(cursorMenu.iPosX + i, cursorMenu.iPosY, cursorMenu.str[i]);
		}
		else
		{
			for (int i = 0; i < strlen(cursorMenu.str); ++i)
				Sprite_Draw(cursorMenu.iPosX + i, cursorMenu.iPosY, ' ');
		}

		visuablitty = !visuablitty;
	}
	else
	{
		for (int i = 0; i < strlen(cursorMenu.str); ++i)
			Sprite_Draw(cursorMenu.iPosX + i, cursorMenu.iPosY, cursorMenu.str[i]);

		for (int i = 0; i < strlen(cursorUp.str); ++i)
			Sprite_Draw(cursorUp.iPosX + i, cursorUp.iPosY, cursorUp.str[i]);

		for (int i = 0; i < strlen(cursorDown.str); ++i)
			Sprite_Draw(cursorDown.iPosX + i, cursorDown.iPosY, cursorDown.str[i]);

		if (cursorUp.focus)
		{
			if (!visuablitty)
			{
				for (int i = 0; i < strlen(cursorUp.str); ++i)
					Sprite_Draw(cursorUp.iPosX + i, cursorUp.iPosY, ' ');
			}
			visuablitty = !visuablitty;
		}

		if (cursorDown.focus)
		{
			if (visuablitty)
			{
				for (int i = 0; i < strlen(cursorDown.str); ++i)
					Sprite_Draw(cursorDown.iPosX + i, cursorDown.iPosY, ' ');
			}

			visuablitty = !visuablitty;
		}
	}
	
	const char* shape = getShapeEnemy(sInfoTemp.eInfo[eIndex].code);

	if (shape != nullptr)
	{
		for (int i = 0; i < strlen(shape); ++i)
		{
			Sprite_Draw(i + 28, MENU_POSY + 3, shape[i]);
		}
	}
	Buffer_Flip();
}


void UpdateStageSetting()
{
	const int NUM_OF_MENU = 5;
	const int NUM_OF_SUB_MENU = 4;
	const int NUM_OF_DESCRIPTION = 8;

	static int page = 0;

	const int countPerPage = 4;

	const int MENU_POSX = 10;
	const int MENU_POSY = 3;
	const int MENU_POSY_DISTANCE = 4;

	const int SUB_MENU_POSX = 70;
	const int SUB_MENU_POSY = 5;
	const int SUB_MENU_POSY_DISTANCE = 2;

	const int CURSOR_POSX_DISTANCE = 4;
	const int SUB_CURSOR_POSX_DISTANCE = 6;

	static int selectedStage = -1;

	static Cusor cusorMenu = { 0, MENU_POSX - CURSOR_POSX_DISTANCE, MENU_POSY, "=>", true };
	static Cusor cusorSubMenu = { 0, SUB_MENU_POSX - SUB_CURSOR_POSX_DISTANCE, SUB_MENU_POSY, "=>", false };
	static Cusor cusorUp = { 0, 105, 0, "->" , false };
	static Cusor cusorDown = { 0, 100, 0, "<-" , false };

	Menu description[NUM_OF_DESCRIPTION]
	{
		{0, 10, 20, "Key Operation <K>"},
		{0, 20, 23, "SELECT : <SPACE>"},
		{0, 50, 23, "SAVE : <ENTER>"},
		{0, 80, 23, "ESCAPE : <ESC>"},
		{0, 20, 25, "NEW : <INSERT>"},
		{0, 50, 25, "DELETE : <DELETE>"},
		{0, 80, 25, "NEXT : <TAB>"},
		{0, 20, 27, "BACK : <BACK SPACE>"}
	};

	Menu subMenu[NUM_OF_SUB_MENU] =
	{
		{SETTING_STAGE_NUMBER, SUB_MENU_POSX, SUB_MENU_POSY ,								"        Stage order : "},
		{SETTING_RESPWAN_INTERVAL, SUB_MENU_POSX, SUB_MENU_POSY + SUB_MENU_POSY_DISTANCE,	"   Respawn Interval : "},
		{SETTING_STAGE_OPEN,SUB_MENU_POSX, SUB_MENU_POSY + (SUB_MENU_POSY_DISTANCE * 2),	"         Stage Open : "},
		{SETTING_PAGE,SUB_MENU_POSX, SUB_MENU_POSY + (SUB_MENU_POSY_DISTANCE * 3),			"       Page setting"},
	};

	int numOfStage;

	numOfStage = sInfoTemp.numOfStage - countPerPage * page;

	if (numOfStage / countPerPage > 0)
		numOfStage = countPerPage;

	Menu menu[NUM_OF_MENU];

	for (int i = 0; i < numOfStage; ++i)
	{
		menu[i].iPosX = MENU_POSX;
		menu[i].iPosY = MENU_POSY + (MENU_POSY_DISTANCE * i);
		menu[i].kind = i + (countPerPage * page);		
		menu[i].str = sInfoTemp.hList[i + (countPerPage * page)].szName;
	}

	if ((GetAsyncKeyState(VK_UP) & 0x8001) != 0)
	{
		if (numOfStage > 0)
		{
			if (cusorMenu.focus)
			{
				if (cusorMenu.iPosY > MENU_POSY)
					cusorMenu.iPosY -= MENU_POSY_DISTANCE;
			}
			else if (cusorSubMenu.focus)
			{
				if (cusorSubMenu.iPosY > SUB_MENU_POSY)
					cusorSubMenu.iPosY -= SUB_MENU_POSY_DISTANCE;
			}
		}

	}
	else if ((GetAsyncKeyState(VK_DOWN) & 0x8001) != 0)
	{
		if (numOfStage > 0)
		{
			if (cusorMenu.focus)
			{
				if (cusorMenu.iPosY < MENU_POSY + (MENU_POSY_DISTANCE * (numOfStage - 1)))
					cusorMenu.iPosY += MENU_POSY_DISTANCE;
			}
			else if (cusorSubMenu.focus)
			{
				if (cusorSubMenu.iPosY < SUB_MENU_POSY + (SUB_MENU_POSY_DISTANCE * (NUM_OF_SUB_MENU - 1)))
					cusorSubMenu.iPosY += SUB_MENU_POSY_DISTANCE;
			}
		}
	}
	else if ((GetAsyncKeyState(VK_LEFT) & 0x8001) != 0)
	{
		if (cusorUp.focus)
		{
			cusorUp.focus = false;
			cusorDown.focus = true;
		}
	}
	else if ((GetAsyncKeyState(VK_RIGHT) & 0x8001) != 0)
	{
		if (cusorDown.focus)
		{
			cusorUp.focus = true;
			cusorDown.focus = false;
		}
	}
	else if ((GetAsyncKeyState(VK_SPACE) & 0x8001) != 0)
	{
		if (numOfStage > 0)
		{
			if (cusorMenu.focus)
			{
				for (int i = 0; i < numOfStage; ++i)
				{
					if ( (menu[i].iPosX == cusorMenu.iPosX + CURSOR_POSX_DISTANCE) && (menu[i].iPosY == cusorMenu.iPosY))
					{
						cusorMenu.focus = false;
						cusorSubMenu.focus = true;
						cusorSubMenu.iPosX = SUB_MENU_POSX - SUB_CURSOR_POSX_DISTANCE;
						cusorSubMenu.iPosY = SUB_MENU_POSY;

						stageFileName = menu[i].str;
						selectedStage = i;
					}
				}
			}
			else if (cusorSubMenu.focus)
			{
				for (int i = 0; i < NUM_OF_SUB_MENU; ++i)
				{
					if ((subMenu[i].iPosX == cusorSubMenu.iPosX + SUB_CURSOR_POSX_DISTANCE) && (subMenu[i].iPosY == cusorSubMenu.iPosY))
					{
						switch (subMenu[i].kind)
						{
						case SETTING_STAGE_NUMBER:
						case SETTING_RESPWAN_INTERVAL:
						case SETTING_STAGE_OPEN:

							cusorSubMenu.focus = false;

							cusorUp.iPosY = subMenu[i].iPosY;
							cusorDown.iPosY = subMenu[i].iPosY;

							cusorUp.focus = true;
							cusorDown.focus = false;

							cusorUp.kind = subMenu[i].kind;
							cusorDown.kind = subMenu[i].kind;
							break;
						case SETTING_PAGE:
							// 스테이지 파일 로드							
							// 지우고 다시 가면 이전 메모리가 있다.
							memset(&stageTemp, 0, sizeof(Stage));
							if (!loadStageInfo(stageFileName, &stageTemp))
								return;
							g_Scence = SCENCE_PAGE_SETTING;
							break;
						}
					}
				}
			}
			else if (cusorUp.focus)
			{
				switch (cusorUp.kind)
				{
				case SETTING_STAGE_NUMBER:
					++sInfoTemp.hList[selectedStage].order;

					break;
				case SETTING_RESPWAN_INTERVAL:
					++sInfoTemp.hList[selectedStage].rpwInterval;
					break;
				case SETTING_STAGE_OPEN:
					++sInfoTemp.hList[selectedStage].isOpen = !sInfoTemp.hList[selectedStage].isOpen;
					break;
				}
			}
			else if (cusorDown.focus)
			{
				switch (cusorDown.kind)
				{
				case SETTING_STAGE_NUMBER:
					if (sInfoTemp.hList[selectedStage].order > 0)
						--sInfoTemp.hList[selectedStage].order;

					break;
				case SETTING_RESPWAN_INTERVAL:
					if (sInfoTemp.hList[selectedStage].rpwInterval > 0)
						--sInfoTemp.hList[selectedStage].rpwInterval;

					break;
				case SETTING_STAGE_OPEN:
					sInfoTemp.hList[selectedStage].isOpen = !sInfoTemp.hList[selectedStage].isOpen;
					break;
				}
			}
		}
	}

	else if ((GetAsyncKeyState(VK_ESCAPE) & 0x8001) != 0)
	{
		cusorMenu.focus = true;
		cusorMenu.iPosX = MENU_POSX - CURSOR_POSX_DISTANCE;
		cusorMenu.iPosY = MENU_POSY;
	
		cusorSubMenu.focus = false;
		cusorSubMenu.iPosX = SUB_MENU_POSX - SUB_CURSOR_POSX_DISTANCE;
		cusorSubMenu.iPosX = SUB_MENU_POSY;

		g_Scence = SCENCE_SETTING;
	}


	else if ((GetAsyncKeyState(VK_TAB) & 0x8001) != 0)
	{
		if (cusorMenu.focus)
		{
			if (sInfoTemp.numOfStage > countPerPage * (page + 1))
			{
				cusorMenu.focus = true;
				cusorMenu.iPosX = MENU_POSX - CURSOR_POSX_DISTANCE;
				cusorMenu.iPosY = MENU_POSY;

				++page;
			}
		}
		else if (cusorSubMenu.focus)
		{
			cusorSubMenu.focus = false;
			cusorMenu.focus = true;
		}
		else if (cusorUp.focus || cusorDown.focus)
		{
			cusorUp.focus = false;
			cusorDown.focus = false;
		}
	}
	else if ((GetAsyncKeyState(VK_BACK) & 0x8001) != 0)
	{
		if (cusorMenu.focus)
		{
			if (page > 0)
			{
				cusorMenu.focus = true;
				cusorMenu.iPosX = MENU_POSX - CURSOR_POSX_DISTANCE;
				cusorMenu.iPosY = MENU_POSY;

				--page;
			}
		}
		else if (cusorSubMenu.focus)
		{
			cusorSubMenu.focus = false;
			cusorSubMenu.iPosX = SUB_MENU_POSX + SUB_CURSOR_POSX_DISTANCE;
			cusorSubMenu.iPosY = SUB_MENU_POSY;
			cusorMenu.focus = true;
		}
		else if (cusorUp.focus || cusorDown.focus)
		{
			cusorSubMenu.focus = true;
			cusorUp.focus = false;
			cusorDown.focus = false;
		}
	}

	else if ((GetAsyncKeyState(VK_RETURN) & 0x8001) != 0)
	{
		if (!saveSettingInfo(&sInfoTemp))
			return;

		g_Scence = SCENCE_SETTING;

		cusorMenu.focus = true;
		cusorMenu.iPosX = MENU_POSX - CURSOR_POSX_DISTANCE;
		cusorMenu.iPosY = MENU_POSY;

		cusorSubMenu.focus = false;
		cusorSubMenu.iPosX = SUB_MENU_POSX - SUB_CURSOR_POSX_DISTANCE;
		cusorSubMenu.iPosX = SUB_MENU_POSY;
	}

	else if ((GetAsyncKeyState(VK_INSERT) & 0x8001) != 0)
	{
		newStage(&sInfoTemp);
	}

	else if ((GetAsyncKeyState(VK_DELETE) & 0x8001) != 0)
	{
		for (int i = 0; i < numOfStage; ++i)
		{
			if (cusorMenu.iPosX + CURSOR_POSX_DISTANCE == menu[i].iPosX && cusorMenu.iPosY == menu[i].iPosY)
			{
				deleteStage(menu[i].str, &sInfoTemp);
				
				cusorMenu.focus = true;

				cusorSubMenu.focus = false;
				cusorSubMenu.iPosX = SUB_MENU_POSX - SUB_CURSOR_POSX_DISTANCE;
				cusorSubMenu.iPosX = SUB_MENU_POSY;

				break;
			}
		}
	}

	else if ((GetAsyncKeyState(VK_PRIOR) & 0x8001) != 0)
	{
		//noting
	}

	else if ((GetAsyncKeyState(VK_NEXT) & 0x8001) != 0)
	{
		//noting
	}

	Buffer_Clear();

	for (int i = 0; i < numOfStage; ++i)
	{
		for (int j = 0; j < strlen(menu[i].str); ++j)
		{
			Sprite_Draw(menu[i].iPosX + j, menu[i].iPosY, menu[i].str[j]);
		}		
	}

	for (int i = 0; i < NUM_OF_DESCRIPTION; ++i)
	{
		for (int j = 0; j < strlen(description[i].str); ++j)
			Sprite_Draw(description[i].iPosX + j, description[i].iPosY, description[i].str[j]);
	}

	if (numOfStage > 0)
	{
		if (!cusorMenu.focus)
		{
			char buf[30];

			for (int i = 0; i < NUM_OF_SUB_MENU; i++)
			{
				int iPosX = subMenu[i].iPosX;
				int iPosY = subMenu[i].iPosY;

				switch (subMenu[i].kind)
				{
				case SETTING_STAGE_NUMBER:
					snprintf(buf, 30, "%s%3d", subMenu[i].str, sInfoTemp.hList[selectedStage].order);
					break;
				case SETTING_RESPWAN_INTERVAL:
					snprintf(buf, 30, "%s%3d", subMenu[i].str, sInfoTemp.hList[selectedStage].rpwInterval);
					break;
				case SETTING_PAGE:
					snprintf(buf, 30, "%s", subMenu[i].str);
					break;
				case SETTING_STAGE_OPEN:
					snprintf(buf, 30, "%s%3d", subMenu[i].str, sInfoTemp.hList[selectedStage].isOpen);
					break;
				}

				for (int j = 0; j < strlen(buf); ++j)
					Sprite_Draw(iPosX + j, iPosY, buf[j]);
			}

		}

		if (cusorMenu.focus)
		{
			if (visuablitty)
			{
				for (int i = 0; i < strlen(cusorMenu.str); ++i)
					Sprite_Draw(cusorMenu.iPosX + i, cusorMenu.iPosY, cusorMenu.str[i]);
			}
			else
			{
				for (int i = 0; i < strlen(cusorMenu.str); ++i)
					Sprite_Draw(cusorMenu.iPosX + i, cusorMenu.iPosY, ' ');
			}

			visuablitty = !visuablitty;
		}
		else if (cusorSubMenu.focus)
		{
			for (int i = 0; i < strlen(cusorMenu.str); ++i)
				Sprite_Draw(cusorMenu.iPosX + i, cusorMenu.iPosY, cusorMenu.str[i]);

			for (int i = 0; i < strlen(cusorSubMenu.str); ++i)
				Sprite_Draw(cusorSubMenu.iPosX + i, cusorSubMenu.iPosY, cusorSubMenu.str[i]);

			if (!visuablitty)
			{
				for (int i = 0; i < strlen(cusorSubMenu.str); ++i)
					Sprite_Draw(cusorSubMenu.iPosX + i, cusorSubMenu.iPosY, ' ');
			}
			visuablitty = !visuablitty;
		}
		else
		{
			for (int i = 0; i < strlen(cusorMenu.str); ++i)
				Sprite_Draw(cusorMenu.iPosX + i, cusorMenu.iPosY, cusorMenu.str[i]);

			for (int i = 0; i < strlen(cusorSubMenu.str); ++i)
				Sprite_Draw(cusorSubMenu.iPosX + i, cusorSubMenu.iPosY, cusorSubMenu.str[i]);

			for (int i = 0; i < strlen(cusorUp.str); ++i)
				Sprite_Draw(cusorUp.iPosX + i, cusorUp.iPosY, cusorUp.str[i]);

			for (int i = 0; i < strlen(cusorDown.str); ++i)
				Sprite_Draw(cusorDown.iPosX + i, cusorDown.iPosY, cusorDown.str[i]);

			if (cusorUp.focus)
			{
				if (!visuablitty)
				{
					for (int i = 0; i < strlen(cusorUp.str); ++i)
						Sprite_Draw(cusorUp.iPosX + i, cusorUp.iPosY, ' ');
				}
				visuablitty = !visuablitty;
			}

			if (cusorDown.focus)
			{
				if (!visuablitty)
				{
					for (int i = 0; i < strlen(cusorDown.str); ++i)
						Sprite_Draw(cusorDown.iPosX + i, cusorDown.iPosY, ' ');
				}

				visuablitty = !visuablitty;
			}
		}	
	}

	Buffer_Flip();
}

/* 선택 후 삭제 기능은 보류 */

void UpdatePageSetting()
{
	const int newEnemy		= 1;
	//const int deleteEnemy	= 2;	

	static Cusor cusorRwp = { newEnemy, 1, 1, SHAPE_ENEMY1, true };

	const char* shape = nullptr;

	const int NUM_OF_DESCRIPTION = 11;

	Menu description[NUM_OF_DESCRIPTION]
	{
		{NONE, 90, 2, "Page : "},
		{NONE, 90, 4, "Num of Enemy : "},
		{NONE, 90, 10, "Key Operation <K>"},
		{NONE, 90, 12, "SELECT : <SPACE>"},
		{NONE, 90, 14, "SAVE : <ENTER>"},
		{NONE, 90, 16, "ESCAPE : <ESC>"},
		//{NONE, 90, 18, "MDOE : <SHIFT>"},
		{NONE, 90, 18, "NEXT : <TAB>"},
		{NONE, 90, 20, "BACK : <BACKSPACE>"},
		{NONE, 90, 22, "PAGE UP : <PAGE UP>"},
		{NONE, 90, 24, "PAGE DOWN : <PAGE DOWN>"},
		{NONE, 90, 26, "PAGE INIT : <DELETE>"},
	};

	static bool visibility = true;	
	static int eIndex = 0;
	const int maxIndex = sInfoTemp.numOfEnemy - 1;

	static int deleteIndex = 0;

	static int page = 0;

	if ((GetAsyncKeyState(VK_UP) & 0x8001) != 0)
	{
		if (cusorRwp.kind == newEnemy)
		{
			if (cusorRwp.iPosY > 0)
				--cusorRwp.iPosY;
		}		
	}
	else if ((GetAsyncKeyState(VK_DOWN) & 0x8001) != 0)
	{
		if (cusorRwp.kind == newEnemy)
		{
			if (cusorRwp.iPosY < GAME_SCREEN_HIGHT - 1)
				++cusorRwp.iPosY;
		}
		
	}
	else if ((GetAsyncKeyState(VK_LEFT) & 0x8001) != 0)
	{
		if (cusorRwp.kind == newEnemy)
		{
			if (cusorRwp.iPosX > 0)
				--cusorRwp.iPosX;
		}
		/*
		else
		{
			while (deleteIndex > 0)
			{
				--deleteIndex;

				if (sStageTemp.pages[page].rList[deleteIndex].code != 0)
				{
					cusorRwp.iPosX = sStageTemp.pages[page].rList[deleteIndex].respawnX;
					cusorRwp.iPosY = sStageTemp.pages[page].rList[deleteIndex].respawnY;
					cusorRwp.str = getShapeEnemyObject(sStageTemp.pages[page].rList[deleteIndex].code);
					break;
				}
			}
		}
		*/
		
	}
	else if ((GetAsyncKeyState(VK_RIGHT) & 0x8001) != 0)
	{
		if (cusorRwp.kind == newEnemy)
		{
			if (cusorRwp.iPosX < GAME_SCREEN_WIDTH - 1)
				++cusorRwp.iPosX;
		}
		else
		{
			/*
			while (deleteIndex < MAX_PAGE_ENEMY)
			{
				++deleteIndex;

				if (sStageTemp.pages[page].rList[deleteIndex].code != 0)
				{
					cusorRwp.iPosX = sStageTemp.pages[page].rList[deleteIndex].respawnX;
					cusorRwp.iPosY = sStageTemp.pages[page].rList[deleteIndex].respawnY;
					cusorRwp.str = getShapeEnemyObject(sStageTemp.pages[page].rList[deleteIndex].code);
					break;
				}
			}
			*/
		}		
	}
	/*
	else if ((GetAsyncKeyState(VK_SHIFT) & 0x8001) != 0)
	{		
		if (sStageTemp.pages[page].numOfEnemy > 0 && cusorRwp.kind == newEnemy)
		{
			cusorRwp.kind = deleteEnemy;
			cusorRwp.iPosX = sStageTemp.pages[page].rList[deleteIndex].respawnX;
			cusorRwp.iPosY = sStageTemp.pages[page].rList[deleteIndex].respawnY;
			cusorRwp.str = getShapeEnemyObject(sStageTemp.pages[page].rList[deleteIndex].code);
		}		
		
		else if (cusorRwp.kind == deleteEnemy)
		{
			cusorRwp.kind = newEnemy;
			cusorRwp.iPosX = 1;
			cusorRwp.iPosY = 1;
			cusorRwp.str = SHAPE_ENEMY1_OBJECT;
			deleteIndex = 0;
		}
		
	}*/
	else if ((GetAsyncKeyState(VK_SPACE) & 0x8001) != 0)
	{
		if ((cusorRwp.kind == newEnemy) && (stageTemp.pages[page].numOfEnemy < MAX_PAGE_ENEMY))
		{
			for (int i = 0; i < MAX_PAGE_ENEMY; ++i)
			{
				if (stageTemp.pages[page].rList[i].code == 0)
				{
					stageTemp.pages[page].rList[i].code = sInfoTemp.eInfo[eIndex].code;
					_time64(&stageTemp.pages[page].rList[i].order);
					stageTemp.pages[page].rList[i].rpwX = cusorRwp.iPosX;
					stageTemp.pages[page].rList[i].rpwY = cusorRwp.iPosY;

					++stageTemp.pages[page].numOfEnemy;
					break;
				}
			}
		}
		/*
		else if (cusorRwp.kind == deleteEnemy)
		{
			memset(&sStageTemp.pages[page].rList[deleteIndex], 0, sizeof(EnemyRespawnInfo));
			--sStageTemp.pages[page].numOfEnemy;

			if (sStageTemp.pages[page].numOfEnemy > 0)
			{
				deleteIndex = 0;

				while (deleteIndex < MAX_PAGE_ENEMY)
				{
					if (sStageTemp.pages[page].rList[deleteIndex].code != 0)
						break;

					++deleteIndex;
				}

				cusorRwp.iPosX = sStageTemp.pages[page].rList[deleteIndex].respawnX;
				cusorRwp.iPosY = sStageTemp.pages[page].rList[deleteIndex].respawnY;
				cusorRwp.str = getShapeEnemyObject(sStageTemp.pages[page].rList[deleteIndex].code);
			}
			else
			{
				cusorRwp.kind = newEnemy;
				cusorRwp.iPosX = 1;
				cusorRwp.iPosY = 1;
				cusorRwp.str = SHAPE_ENEMY1_OBJECT;
				deleteIndex = 0;
			}
		}
		*/
	}
	else if ((GetAsyncKeyState(VK_ESCAPE) & 0x8001) != 0)
	{
		g_Scence = SCENCE_STAGE_SETTING;
	
		cusorRwp.iPosX = 1;
		cusorRwp.iPosY = 1;
		page = 0;
		eIndex = 0;
		cusorRwp.str = SHAPE_ENEMY1;
	}

	else if ((GetAsyncKeyState(VK_TAB) & 0x8001) != 0)
	{
		if (cusorRwp.kind == newEnemy)
		{
			while (eIndex < maxIndex)
			{
				++eIndex;

				shape = getShapeEnemy(sInfoTemp.eInfo[eIndex].code);
				
				if (shape != nullptr)
				{
					cusorRwp.str = shape;
					break;
				}					
			}			
		}
	}

	else if ((GetAsyncKeyState(VK_BACK) & 0x8001) != 0)
	{
		if (cusorRwp.kind == newEnemy)
		{
			while (eIndex > 0 )
			{				
				--eIndex;
				shape = getShapeEnemy(sInfoTemp.eInfo[eIndex].code);

				if (shape != nullptr)
				{
					cusorRwp.str = shape;
					break;
				}
			}
		}
	}

	else if ((GetAsyncKeyState(VK_RETURN) & 0x8001) != 0)
	{
		if (!saveStageInfo(stageFileName, &stageTemp))
			return;		
		g_Scence = SCENCE_STAGE_SETTING;
		cusorRwp.iPosX = 1;
		cusorRwp.iPosY = 1;
		page = 0;
		eIndex = 0;
		cusorRwp.str = SHAPE_ENEMY1;
	}

	else if ((GetAsyncKeyState(VK_INSERT) & 0x8001) != 0)
	{
		// nothing
	}

	else if ((GetAsyncKeyState(VK_DELETE) & 0x8001) != 0)
	{
		cusorRwp.kind = newEnemy;
		cusorRwp.iPosX = 1;
		cusorRwp.iPosY = 1;
		cusorRwp.str = SHAPE_ENEMY1;		
		//deleteIndex = 0;
		memset(&stageTemp.pages[page], 0, sizeof(Page));
	}

	else if ((GetAsyncKeyState(VK_PRIOR) & 0x8001) != 0)
	{
		if (page < MAX_PAGE - 1)
			++page;
	}

	else if ((GetAsyncKeyState(VK_NEXT) & 0x8001) != 0)
	{
		if (page > 0)
			--page;
	}

	Buffer_Clear();

	for (int i = 0; i < GAME_SCREEN_HIGHT; ++i)
	{
		Sprite_Draw(0, i, '|');
		Sprite_Draw(GAME_SCREEN_WIDTH - 2, i, '|');
	}

	for (int i = 0; i < MAX_PAGE_ENEMY; ++i)
	{
		if (stageTemp.pages[page].rList[i].code == 0)
			continue;

		shape = getShapeEnemy(stageTemp.pages[page].rList[i].code);

		for (int j = 0; j < strlen(shape); ++j)
		{
			Sprite_Draw(stageTemp.pages[page].rList[i].rpwX + j, stageTemp.pages[page].rList[i].rpwY, shape[j]);
		}
	}

	for (int i = 0; i < strlen(cusorRwp.str); ++i)
	{
		Sprite_Draw(cusorRwp.iPosX + i, cusorRwp.iPosY, cusorRwp.str[i]);
	}

	if (cusorRwp.focus)
	{
		if (visibility)
		{
			for (int i = 0; i < strlen(cusorRwp.str); ++i)
			{
				Sprite_Draw(cusorRwp.iPosX + i, cusorRwp.iPosY, ' ');
			}
		}
		visibility = !visibility;
	}

	char szBuf[30];

	sprintf(szBuf, "%s : %d/%d", description[0].str, page + 1, MAX_PAGE);
	
	for (int i = 0; i < strlen(szBuf); ++i)
	{
		Sprite_Draw(description[0].iPosX + i, description[0].iPosY, szBuf[i]);
	}
	
	sprintf(szBuf, "%s : %d/%d", description[1].str, stageTemp.pages[page].numOfEnemy, MAX_PAGE_ENEMY);

	for (int i = 0; i < strlen(szBuf); ++i)
	{
		Sprite_Draw(description[1].iPosX + i, description[1].iPosY, szBuf[i]);
	}

	for (int i = 2; i < NUM_OF_DESCRIPTION; ++i)
	{
		for (int j = 0; j < strlen(description[i].str); ++j)
		{
			Sprite_Draw(description[i].iPosX + j, description[i].iPosY, description[i].str[j]);
		}
	}
	Buffer_Flip();
}