#pragma once
#include <DirectXMath.h>
#include <list>
#include "HitManager.h"
#include "HitShapes.h"
#include "CharacterData.h"
#include "PipeLineManager.h"

using namespace std;

class GameObject;								//GameObjectクラスの宣言は後だけども、GameComponentクラスで宣言に使いたいので前方宣言
class HitQuad;								//GameObject.hと相互読み込みになっているHitManager.hのクラス。前方宣言しないとヘッダが読み込めない。
												//ただし、他hファイルのクラスを前方宣言で使う場合、このh内では前方宣言クラスの実体型は使えない。

//======Change Scene
// 削除用追加処理ステートオブジェクト
class AbstractGameObjectDeleter
{
public:
	virtual void ExecuteDeleter(GameObject* go) = 0;
};
//======Change Scene END

class GameComponent								//インタフェースっぽいけど抽象クラス（メンバ変数があるから）しかし、これ単体では何にも使えない
{
private:
	bool m_activeFlg;
	GameObject* m_gObject;

	virtual void InitAction() = 0;		//コンポーネント初期化時に呼ばれる処理

public:
	virtual ~GameComponent() = default;
	void initFromGameObject(GameObject* objData)
	{
		m_gObject = objData;
		m_activeFlg = true;
		InitAction();
	}

	bool isActive()
	{
		return m_activeFlg;
	}

	void setActive(bool flg)
	{
		m_activeFlg = flg;
	}

	//純粋仮想関数（メソッド）。このクラスでは実装出来ない　継承した別クラスで実装する必要がある
	virtual bool FrameAction() = 0;		//毎フレーム呼ばれる処理　falseを返すとこのコンポーネントは終了し削除される
	virtual void FinishAction() = 0;	//終了時に呼ばれる処理

	//==========HitSystem========
	//ヒット時リアクション処理
	virtual void hitReaction(GameObject* targetGo, HitAreaBase * hit) {};
	//==========HitSystem========

	bool IsActive = true;	//コンポーネントの有効無効

	GameObject* getGameObject() { return m_gObject; }
};

class GameObject								//サンプルエンジン用ゲームオブジェクト基底クラス
{
protected:
	bool m_enableFlg;

	unique_ptr<CharacterData> characterData;	//キャラクタの座標とUVなどが入っている
	list<GameComponent*> components;			//このゲームオブジェクトに乗せられたコンポーネントのリスト

	//======Change Scene
	//Deleterオブジェクトのメンバ変数を追加
	AbstractGameObjectDeleter* m_pDeleter = nullptr;
	//======Change Scene END

public:
	GameObject(CharacterData* cData)
	{
		m_enableFlg = true;
		characterData.reset(cData);
	}

	CharacterData* getCharacterData()			//これでCharacterDataのインスタンスを取得して座標などが直接変えられるようにはしている
	{
		return characterData.get();
	}

	void addComponent(GameComponent* com)		//GameObjectにコンポーネントを足す（処理をふやす）
	{
		com->initFromGameObject(this);					//コンポーネントごとの初期化処理
		components.push_back(com);
	}

	list<GameComponent*>& getComponents()
	{
		return components;
	}

	void removeComponent(GameComponent* com)	//GameObjectからコンポーネントを消す（処理を減らす）
	{
		com->FinishAction();					//コンポーネントごとの終了処理
		components.remove(com);
		delete(com);							//コンポーネントのメモリ解放
	}

	bool isEnable()
	{
		return m_enableFlg;
	}

	void setEnable(bool flg)
	{
		m_enableFlg = flg;
	}

	//======Change Scene
	//Deleterオブジェクトの設定メソッド
	void SetGameObjectDeleter(AbstractGameObjectDeleter* deleter) {
		m_pDeleter = deleter;
	}
	//======Change Scene END

	virtual void cleanupGameObject();			//GameObjectの削除処理コンポーネントの全削除　virtualがついているとoverrideが出来る
	virtual bool action();						//毎フレーム呼ばれるメソッド falseを返すとオブジェクトの消去
	virtual void init();						//GameEngineにADDされた時に呼ばれるメソッド

};
