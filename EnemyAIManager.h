#pragma once

#include "GameObject.h"
#include "FieldCharacter.h"
#include "BattleFieldManager.h"
#include "EnemyTypes.h"
#include "TimeManager.h"

#include <vector>

class EnemyAIManager:public GameComponent
{
private:
	int m_MoveCount = 0;		//移動歩数

	bool m_OnlyOneTime = false;

	bool m_Firsttime = false;

	int m_EnemyIndex = 0;

	int m_MoveAIcount = 0;

	float m_DelayCount = 0.0f;

	vector<float> nextPosList = 
	{
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f
	};

	TimeManager* m_TimeManager;

	EnemyMove Emove = EnemyMove::None;
	AIMode Mode = AIMode::decide;
public:
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;

	bool checkEnemyData();

	void ResetAI(FieldCharacter* chara);

	void OnChangeTurn();

	Squares* GetnearCharaPos(float renge, float charaposX, float charaposY);
	float GetMoveAiCount() { return m_MoveAIcount; }
	float GetDelayCount() { return m_DelayCount; }
};

