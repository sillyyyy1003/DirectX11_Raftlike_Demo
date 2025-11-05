#pragma once
#include "UIBar.h"

class HungerComponent :
    public Component
{
public:
    static constexpr MyComponent::ComponentType TYPE = MyComponent::ComponentType::Hunger;

    HungerComponent(float initialValue);
    ~HungerComponent() override;

    void Init(float starveSpeed, float starveThreshold);

    /// @brief 
    void UnInit();
    void Update(float dt);

    /// @brief 空腹スビートを設定する
    void SetStarveSpeed(float speed);

    /// @brief 空腹度を回復する
    void RestoreHunger(float foodValue);

    bool GetIsStarve() const { return m_isStarve; }
    bool GetIsHungry() const { return m_isHungry; }

    /// @brief 今の空腹度を返す
    float GetCurrentHungerValue() const { return m_currentHungerValue; }
    float GetCurrentHungerPercentage() const;

    void SetStarveThreshold(float threshold) { m_starveThreshold = threshold; }
    

    typedef std::function<void(bool)> Callback;
    /// @brief 空腹状態が変化したときに呼び出されるコールバックを追加
    /// @param cb コールバック関数
    void AddHungryListener(Callback cb)
    {
        m_hungryListeners.push_back(cb);
    }
    /// @brief 空腹状態が変化したときに呼び出されるコールバック
    /// @param isHungry true:空腹状態, false:空腹状態ではない
    void NotifyHungryListeners(bool isHungry)
    {
        for (const auto& listener : m_hungryListeners)
        {
            listener(isHungry);
        }
    }

    /// @brief if starveValue is below 0
    /// @param cb 
    void AddStarveListener(Callback cb)
    {
        m_starveListeners.push_back(cb);
    }

	/// @brief 空腹状態が変化したときに呼び出されるコールバック
	/// @param isStarve m_currentHungerValue is below 0
    void NotifyStarveListener(bool isStarve)
    {
	    for(const auto& listener: m_starveListeners)
        {
            listener(isStarve);
		}
    }

private:
    float m_initialHungerValue;
    float m_currentHungerValue;
    float m_starveThreshold;

    float m_starveSpeed;        // Speed of starving
	bool m_isHungry = false;    // True if player is hungry m_currentHungerValue is below m_starveThreshold
	bool m_isStarve = false;    // True if m_currentHungerValue is below 0


    std::vector<Callback> m_hungryListeners;
    std::vector<Callback> m_starveListeners;
};

