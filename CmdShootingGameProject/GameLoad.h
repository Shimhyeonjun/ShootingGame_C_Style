#pragma once
#ifndef GAME_LOAD_H
#define GAME_LOAD_H

#define	FILE_NAME					"spaceMarinSetting.ini"
#define MAX_STAGE_NAME				256
#define MAX_ENEMY_RESOURCE			10

#include "GameDef.h"

#pragma pack(push, 1)
struct PlayerInfo										
{
	int DefaultHp;
	int DefaultX;
	int DefaultY;
	int DefaultMvDelay;
	int DefaultAtDelay;
	int DefaultBulletMvDelay;
	int DefaultBulletDamege;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct EnemyInfo
{
	int code;
	int DefaultHp;
	int DefaultMvDelay;
	int DefaultAtDelay;
	int DefaultBulletMvDelay;
	int DefaultBulletDamege;
	int DefaultScore;
};
#pragma pack(pop)

struct StageHeader
{
	int order;
	int rpwInterval;								
	bool isOpen;										
	char szName[MAX_STAGE_NAME];
};

#pragma pack(push, 1)
struct SettingInfo										
{
	int numOfStage;										
	StageHeader hList[MAX_STAGE];
	PlayerInfo pInfo;
	int numOfEnemy;
	EnemyInfo eInfo[MAX_ENEMY_RESOURCE];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct EnemyRespawnInfo								
{
	int code;										
	__int64 order;
	int rpwX;										
	int rpwY;										
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Page												
{
	int numOfEnemy;										
	EnemyRespawnInfo rList[MAX_PAGE_ENEMY];				
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Stage											
{
	Page pages[MAX_PAGE];								
};
#pragma pack(pop)

struct EnemyResource									
{
	int numOfResource;									
	int code[MAX_ENEMY_RESOURCE];						
};

bool loadResource(EnemyResource* resource);

bool loadSettingInfo(SettingInfo* sInfo);
bool saveSettingInfo(SettingInfo* sInfo);

bool loadStageInfo(const char* fileName, Stage* stage);
bool saveStageInfo(const char* fileName, Stage* stage);

bool newStage(SettingInfo* sInfo);
bool deleteStage(const char* fileName, SettingInfo* sInfo);

#endif 
