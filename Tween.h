#pragma once

#include <functional>
#include <algorithm>

class Tween
{
private:
    float m_StartValue;
    float m_EndValue;
    float m_Duration;
    float m_ElapsedTime;
    bool m_IsActive;

    std::function<void(float)> m_Setter;
    std::function<float(float, float, float, float)> m_EasingFunc;

public:
    void Update(float deltaTime);

    // アニメーションが終了したかを取得するゲッター
    bool IsActive() const 
    { 
        return m_IsActive;
    }

    // コンストラクタ
    Tween(float start, 
        float end, 
        float dur, 
        const std::function<void(float)>& setterFunc, 
        const std::function<float(float, float, float, float)>& easing) : 
        m_StartValue(start), m_EndValue(end), m_Duration(dur), 
        m_ElapsedTime(0.0f), m_IsActive(true), m_Setter(setterFunc), m_EasingFunc(easing)
    {
    }

    // 線形補間
    static float EaseLinear(float t, float b, float c, float d)
    {
        return c * t / d + b;
    }

    // 加速（Quad In）
    static float EaseInQuad(float t, float b, float c, float d)
    {
        t /= d;
        return c * t * t + b;
    }

    // 減速（Quad Out）
    static float EaseOutQuad(float t, float b, float c, float d)
    {
        t /= d;
        return -c * t * (t - 2) + b;
    }
};

