#pragma once

#include <Windows.h>

class SceneController
{
protected:
	UINT m_scene = 0;

	//=======Change Scene
	UINT m_orderSceneID; //予約された次のシーン番号
	//=======Change Scene END

public:
	//virtualにしているのはオリジナルのシーン構造を作る時にオーバライド出来るように。
	HRESULT virtual initSceneController() = 0;
	HRESULT virtual changeGameScene(UINT scene) = 0;

	//=======Change Scene
	void OrderNextScene(UINT scene) //シーン切り替え予約
	{
		m_orderSceneID = scene;
	}
	void CheckSceneOrder() //シーン切り替え実行
	{
		if (m_scene != m_orderSceneID) //今のシーン番号と予約番号が異なるなら切り替え
		{
			if (SUCCEEDED(changeGameScene(m_orderSceneID)))
			{
				m_scene = m_orderSceneID;
			}
		}
	}
	//=======Change Scene END
};
