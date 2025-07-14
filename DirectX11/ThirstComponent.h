#pragma once
#include "Component.hpp"
#include "UIBar.h"

class ThirstComponent :
    public Component
{
public:
    ThirstComponent(float initialValue);
    ~ThirstComponent() override = default;

	void Update(float dt);

    /// @brief 渇き度を設定
	/// @param speed Speed of thirst per second
    void SetThirstSpeed(float speed);

    /// @brief 渇きを回復する
    void RestoreThirst(float thirstValue);

	bool GetIsThirsty() { return m_isThirsty; }

	float GetCurrentThirstValue() const { return m_currentThirstValue; }

	void SetUIComponent(UIBar* thirstBar);

    /// @brief UI描画
    void Draw();
private:
    float m_initialThirstValue;
	float m_currentThirstValue;

    float m_thirstSpeed;
    bool m_isThirsty = false;

	UIBar* m_pUiComponent;

};

