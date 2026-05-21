#include "Tween.h"

void Tween::Update(float deltaTime)
{
    if (!m_IsActive) return;

    m_ElapsedTime += deltaTime;

    float timePersent = std::min(m_ElapsedTime, m_Duration);
    float startValue = m_StartValue;
    float changeValue = m_EndValue - m_StartValue;
    float duration = m_Duration;

    // イージング関数を使って現在の値を計算
    float currentValue = m_EasingFunc(timePersent, startValue, changeValue, duration);

    // 対象のプロパティに値を設定
    m_Setter(currentValue);

    // アニメーション終了判定
    if (m_ElapsedTime >= m_Duration)
    {
        // 終了値に確実に設定
        m_Setter(m_EndValue);
        m_IsActive = false;
    }
}
