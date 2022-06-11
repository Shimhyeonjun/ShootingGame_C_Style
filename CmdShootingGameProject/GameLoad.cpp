#define _CRT_SECURE_NO_WARNINGS
#include "GameLoad.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

bool loadResource(EnemyResource* resource)
{
	// 외부에서 리소스가 있다고 가정
	int i = 0;

	resource->code[i++] = CODE_ENEMY1;
	resource->code[i++] = CODE_ENEMY2;
	resource->code[i++] = CODE_ENEMY3;
	resource->code[i++] = CODE_ENEMY4;

	resource->numOfResource = i;

	return true;
}

// 설정 파일 Load
bool loadSettingInfo(SettingInfo* sInfo)
{
	FILE* pFile = fopen(FILE_NAME, "rb");

	if (pFile == nullptr)
		return false;

	size_t readSize = fread(&sInfo->numOfStage, 1, sizeof(sInfo->numOfStage), pFile);
	if (readSize != sizeof(sInfo->numOfStage))
	{
		fclose(pFile);
		return false;
	}
	
	readSize = fread(sInfo->hList, 1, sizeof(StageHeader) * sInfo->numOfStage, pFile);
	if (readSize != sizeof(StageHeader) * sInfo->numOfStage)
	{
		fclose(pFile);
		return false;
	}

	readSize = fread(&sInfo->pInfo, 1, sizeof(PlayerInfo), pFile);
	if (readSize != sizeof(PlayerInfo))
	{
		fclose(pFile);
		return false;
	}
	
	readSize = fread(&sInfo->numOfEnemy, 1, sizeof(sInfo->numOfEnemy) , pFile);
	if (readSize != sizeof(sInfo->numOfEnemy))
	{
		fclose(pFile);
		return false;
	}
	
	readSize = fread(sInfo->eInfo, 1, sizeof(EnemyInfo) * sInfo->numOfEnemy, pFile);
	if (readSize != sizeof(EnemyInfo) * sInfo->numOfEnemy)
	{
		fclose(pFile);
		return false;
	}

	fclose(pFile);
	return true;
}

bool saveSettingInfo(SettingInfo* sInfo)
{
	FILE* pFile = fopen(FILE_NAME, "wb");

	if (pFile == nullptr)
		return false;

	size_t writtenSize = fwrite(&sInfo->numOfStage, 1, sizeof(sInfo->numOfStage), pFile);
	if (writtenSize != sizeof(sInfo->numOfStage))
	{
		fclose(pFile);
		return false;
	}

	writtenSize = fwrite(sInfo->hList, 1, sizeof(StageHeader) * sInfo->numOfStage, pFile);
	if (writtenSize != sizeof(StageHeader) * sInfo->numOfStage)
	{
		fclose(pFile);
		return false;
	}

	writtenSize = fwrite(&sInfo->pInfo, 1, sizeof(PlayerInfo), pFile);
	if (writtenSize != sizeof(PlayerInfo))
	{
		fclose(pFile);
		return false;
	}

	writtenSize = fwrite(&sInfo->numOfEnemy, 1, sizeof(sInfo->numOfEnemy), pFile);
	if (writtenSize != sizeof(sInfo->numOfEnemy))
	{
		fclose(pFile);
		return false;
	}

	writtenSize = fwrite(sInfo->eInfo, 1, sizeof(EnemyInfo) * sInfo->numOfEnemy, pFile);
	if (writtenSize != sizeof(EnemyInfo) * sInfo->numOfEnemy)
	{
		fclose(pFile);
		return false;
	}

	fclose(pFile);
	return true;
}

bool loadStageInfo(const char* fileName, Stage* stage)
{
	FILE* pFile = fopen(fileName, "rb");

	if (pFile == nullptr)
		return false;
	
	fread(stage->pages, 1, sizeof(Stage), pFile);

	fclose(pFile);

	return true;	
}

bool saveStageInfo(const char* fileName, Stage* stage)
{
	FILE* pFile = fopen(fileName, "wb");
	
	if (pFile == nullptr)
		return false;

	size_t writtenSize = fwrite(stage->pages, 1, sizeof(Stage), pFile);
	if (writtenSize != sizeof(Stage))
		return false;

	fclose(pFile);

	return true;
}


bool newStage(SettingInfo* sInfo)
{
	char newFileName[MAX_STAGE_NAME];
	bool isExist = false;

	struct tm localTime;
	__time64_t longTime;
	
	do
	{
		isExist = false;

		_time64(&longTime);
		_localtime64_s(&localTime, &longTime);

		snprintf(newFileName, MAX_STAGE_NAME, "%s_%04d%02d%02d_%02d%02d%02d.dat", "Stage", localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday,
			localTime.tm_hour, localTime.tm_min, localTime.tm_sec);

		for (int j = 0; j < sInfo->numOfStage; ++j)
		{
			if (!strcmp(newFileName, sInfo->hList[j].szName))
			{
				isExist = true;
				break;
			}
		}
	} while (isExist);

	memcpy(&sInfo->hList[sInfo->numOfStage++].szName, newFileName, strlen(newFileName) + 1);
	
	if (!saveSettingInfo(sInfo))
		return false;

	FILE* pFile = fopen(newFileName, "wb");

	if (pFile == nullptr)
		return false;

	fclose(pFile);
	
	return true;
}

bool deleteStage(const char* fileName, SettingInfo* sInfo)
{
	int target = -1;
	char targetName[FILENAME_MAX];

	strcpy(targetName, fileName);

	for (int i = 0; i < sInfo->numOfStage; ++i)
	{
		if (!strcmp(targetName, sInfo->hList[i].szName))
		{
			memset(&sInfo->hList[i], 0, sizeof(StageHeader));
			target = i;
			break;
		}			
	}

	if (target == -1)
		return false;

	for (int j = target; j < sInfo->numOfStage - 1; ++j)
	{	
		memcpy(sInfo->hList[j].szName, sInfo->hList[j + 1].szName, strlen(sInfo->hList[j + 1].szName) + 1);
	}

	--sInfo->numOfStage;

	if (!saveSettingInfo(sInfo))
		return false;
	
	if (remove(targetName))
		return false;

	return true;
}
