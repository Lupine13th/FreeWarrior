#include "MyAccessHub.h"
//#include "BattleFieldManager.h"

//staticメンバ変数は初期化が必要。
//C++はなんと宣言部の外に書く必要がある。C#と大きく異るので注意。
//ヘッダに書いちゃうと、複数のcppで読み込むヘッダだとリンクエラーになるので注意。
MyGameEngine* MyAccessHub::m_engine = nullptr;
BattleFieldManager* MyAccessHub::m_BattleFieldManager = nullptr;
EnemyAIManager* MyAccessHub::m_EnemyAIManager = nullptr;
BattleReadyScene* MyAccessHub::m_BattleReadyScene = nullptr;
FlyingCameraController* MyAccessHub::m_FlyingCameraController = nullptr;
TimeManager* MyAccessHub::m_TimeManager = nullptr;
TitleScene* MyAccessHub::m_TitleScene = nullptr;
HUDManager* MyAccessHub::m_HUDManager = nullptr;
EffectGenerator* MyAccessHub::m_EffectGenerator = nullptr;
