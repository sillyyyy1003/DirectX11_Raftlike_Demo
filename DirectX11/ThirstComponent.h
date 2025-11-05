#pragma once
#include "Component.hpp"
#include "UIBar.h"

class ThirstComponent :
    public Component
{
public:
    static constexpr MyComponent::ComponentType TYPE = MyComponent::ComponentType::Thirst;

    ThirstComponent(float initialValue);
    ~ThirstComponent() override = default;

    void Init(float thirstSpeed, float thirstThreshold);

	void Update(float dt);

    /// @brief 渇き度を設定
	/// @param speed Speed of thirst per second
    void SetThirstSpeed(float speed);

    /// @brief 渇きを回復する
    void RestoreThirst(float thirstValue);

	bool GetIsThirsty() { return m_isThirsty; }
    bool GetIsThirstyToDeath()const { return m_isThirstyToDeath; }

	/// @brief 今の渇き度を返す
    float GetCurrentThirstValue() const { return m_currentThirstValue; }

    void SetThirstThreshold(float threshold) { m_thirstThreshold = threshold; }

    float GetCurrentThirstPercentage()const;


    typedef std::function<void(bool)> Callback;
    /// @brief 渇き度が変化したときに呼び出されるコールバックを追加
    /// @param cb コールバック関数
    void AddThirstyListener(Callback cb)
    {
        m_thirstyListeners.push_back(cb);
    }
    /// @brief 渇き度が変化したときに呼び出されるコールバック
    /// @param isThirsty true:is thirsty
    void NotifyThirstyListeners(bool isThirsty)
    {
        for (const auto& listener : m_thirstyListeners)
        {
            listener(isThirsty);
        }
    }


    /// @brief if thirstValue is below 0
    /// @param cb 
    void AddThirstyToDeathListener(Callback cb)
    {
        m_thirstyToDeathListeners.push_back(cb);
    }

    /// @brief 渇き度が変化したときに呼び出されるコールバック
    /// @param isThirstyToDeath m_ thirstHungerValue is below 0
    void NotifyThirstyToDeathListener(bool isThirstyToDeath)
    {
        for (const auto& listener : m_thirstyToDeathListeners)
        {
            listener(isThirstyToDeath);
        }
    }



private:
	float m_initialThirstValue; // 初期渇き度
	float m_currentThirstValue; // 現在の渇き度
	float m_thirstThreshold;    // 渇き度の閾値

    float m_thirstSpeed;
	bool m_isThirsty = false;        // True>> m_currentThirstValue <= m_thirstThreshold
	bool m_isThirstyToDeath = false;    // True>> m_currentThirstValue <= 0


    std::vector<Callback> m_thirstyListeners;
    std::vector<Callback> m_thirstyToDeathListeners;

};

