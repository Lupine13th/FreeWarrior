#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include <list>
#include "GameObject.h"

#include "HitShapes.h"

using namespace DirectX;
using namespace std;

class GameComponent;

//enum class HIT_AREA_TYPE	//判定の種類列挙子
//{
//	HIT_PLAYER_BODY = 0x00000001UL,		//自機の当たり判定
//	HIT_PLAYER_ATTACK = 0x00000002UL,		//自機の攻撃（本体、弾）
//	HIT_PLAYER_SHIELD = 0x00000004UL,		//自機のバリア（今回使わないけど）
//	HIT_ENEMY_BODY = 0x00000010UL,		//敵の当たり判定
//	HIT_ENEMY_ATTACK = 0x00000020UL,		//敵の攻撃（本体、弾）
//	HIT_ENEMY_SHIELD = 0x00000040UL,		//敵のバリア（今回使わないけど）
//};

class HitManager
{
private:
    class HitStructure	//クラス内クラス　こういう事も出来るんでややこしいプログラムはほんとにややこしくなる。
    {
    private:
		HitAreaBase* m_pHitArea;							//判定領域データ
        GameComponent* m_pGameComponent;          //判定を設定したGameObject
    public:
        HitStructure(GameComponent* cmp, HitAreaBase* hitArea)
        {
            m_pHitArea = hitArea;
            m_pGameComponent = cmp;
        }

        //bool isHit(HitStructure* target);			//解説した当たり判定チェックを行うメソッド　trueでヒット
        GameComponent* getGameComponent();			//判定を設定したGameObjectを取得
		HitAreaBase* getHitArea();						//登録してある判定データ本体を取得
    };

	list<DirectX::XMUINT2> m_hitOrders;
	
    //一つのリストに固めるの手だけども、最初から種類ごとにわける事で処理数を減らせる。
    //例えば、敵武器判定は敵本体や自機武器の判定には当たらない。あたったら勝手に敵がやられるよね。
    UINT m_numOfHitTypes = 0;
    vector<unique_ptr<list<HitStructure*>>> m_hitArray;

    void flushHitList(list<HitStructure*>* p_hitlist);  //判定保存用listのクリア　refreshHitSystemはこれを全部に行う

    XMFLOAT3    m_lastHitPos;

public:
	void initHitList(UINT hitTypes);
	void setHitOrder(UINT atk, UINT def);

    void refreshHitSystem();                            //ヒットシステムデータクリア　フレーム開始時に実行する。
    void setHitArea(GameComponent * cmp, HitAreaBase* box);  //当たり判定システムに判定データを登録する

    void hitFrameAction();                              //判定一斉処理　登録順の前後で結果が変わらないようにしている

    bool isHit(HitAreaBase* atkHit, HitAreaBase* defHit);

    XMFLOAT3 getLastHitPoint()
    {
        return m_lastHitPos;
    }
};
