#pragma once
#include "UIBar.h"

class HungerComponent:
	public Component
{
public:
    HungerComponent(float initialValue);
    ~HungerComponent() override = default;

    void Update(float dt);

    /// @brief 空腹スビートを設定する
    void SetStarveSpeed(float speed);

    /// @brief 空腹度を回復する
    void RestoreHunger(float foodValue);

    bool GetIsStarve() { return m_isStarve; }

    /// @brief 今の空腹度を返す
    float GetCurrentHungerValue() const { return m_currentHungerValue; }

    void SetUIComponent(UIBar* pUi);

    void Draw();

private:
    float m_initialHungerValue;
    float m_currentHungerValue;

    float m_starveSpeed;        // Speed of starving
    bool m_isStarve = false;

    UIBar* m_pUiComponent;

};

