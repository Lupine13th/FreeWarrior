#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"

#include <vector>
#include <chrono>


class MenuUI : public GameComponent
{
private:
    vector<std::unique_ptr<SpriteCharacter>> m_sprite;

    vector<float> posx = {-450.0f, -410.0f, -370.0f, -330.0f, -290.0f, -250.0f}; //0:1文字目 1:2文字目 2:3文字目 3:4文字目 4:5文字目

    vector<float> posy = {180.0f, 100.0f, 20.0f, -60.0f, -140.0f}; //0:攻撃 1:移動 2:行動 3:待機 4:キャンセル 

    float m_PosZ = 2.5f;

    bool OpenAnim = false;
    //bool CloseAnim = false;

    float MenuAnimCount = 0.0f;

    float persent = 1.0f;

    std::chrono::steady_clock::time_point lastTime;
public:
    bool MenuUIenable = false;

    bool CloseAnim = false;

    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;

    void OpenMenuAnim();
    void CloseMenuAnim();
};

