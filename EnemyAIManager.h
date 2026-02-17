#pragma once

#include "GameObject.h"
#include "FieldCharacter.h"
#include "BattleFieldManager.h"
#include "EnemyTypes.h"
#include "TimeManager.h"

#include <vector>

// 行動の種類を拡張する
enum class AIActionType
{
	None,
	Move,
	Attack,
	UseSkill, // スキル使用も考慮に入れる
	Wait,
};

struct EnemyAction
{
	AIActionType m_ActionType = AIActionType::None;
	int m_TargetSqureaID = -1;		//攻撃、スキルの対象となるマスID
	int m_TargetCharacterID = -1;	//攻撃、スキルの対象となるキャラクターID
	int m_MoveSquareID = -1;		//移動先のマスID
};

class EnemyAIManager:public GameComponent
{
private:
	int m_MoveCount = 0;		//移動歩数

	bool m_OnlyOneTime = false;

	bool m_Firsttime = false;

	int m_EnemyIndex = 0;

	int m_MoveAIcount = 0;

	float m_DelayCount = 0.0f;

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

	//現在のキャラクターが出来る行動を生成
	vector<EnemyAction> GeneratePossibleActions(FieldCharacter* currentCharacter);

	//生成した行動を評価
	float EvaluateAction(FieldCharacter* currentCharacter, const EnemyAction& action);

	//生成された行動から最も評価の高い行動を選択
	EnemyAction SelectBestAction(FieldCharacter* currentEnemy, const std::vector<EnemyAction>& possibleActions);

	//二点からマンハッタン距離を計算
	float CalculateDistance(int currentID, int nextID);
};

