#include "GameObject.h"
#include <MyAccessHub.h>
//#include <algorithm>

void GameObject::init()
{
}

bool GameObject::action()	//フレーム処理
{
	list<GameComponent*> deleteComponents;
	if (!isEnable()) return true;

	for (auto comp : components)
	{
		if (comp->isActive() && !comp->FrameAction())
		{
			deleteComponents.push_back(comp);
		}
	}

	//終了コンポーネント削除モード
	if (!deleteComponents.empty())
	{
		for (auto deleteComp : deleteComponents)			
		{
			removeComponent(deleteComp);					
		}
		deleteComponents.clear();
		if (components.empty())
		{
			return false;
		}
	}
	return true;
}

void GameObject::cleanupGameObject()							//完全終了処理
{
	for (auto comp : components)
	{
		comp->FinishAction();
		delete(comp);
	}

	components.clear();
	//Deleter実行
	if (m_pDeleter != nullptr)
	{
		m_pDeleter->ExecuteDeleter(this);
	}
}
