#include "MyAccessHub.h"
//#include "BattleFieldManager.h"

//staticメンバ変数は初期化が必要。
MyGameEngine* MyAccessHub::m_engine = nullptr;
BattleFieldManager* MyAccessHub::m_BattleFieldManager = nullptr;
EnemyAIManager* MyAccessHub::m_EnemyAIManager = nullptr;
BattleReadyScene* MyAccessHub::m_BattleReadyScene = nullptr;
FlyingCameraController* MyAccessHub::m_FlyingCameraController = nullptr;
TimeManager* MyAccessHub::m_TimeManager = nullptr;
TitleScene* MyAccessHub::m_TitleScene = nullptr;
HUDManager* MyAccessHub::m_HUDManager = nullptr;
EffectGenerator* MyAccessHub::m_EffectGenerator = nullptr;
