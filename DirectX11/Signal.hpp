#pragma once
#include <vector>
#include <functional>

template<typename... Args>
class Signal
{
public:
	using Slot = std::function<void(Args...)>;
	typedef std::vector<Slot> Slots;
private:

	Slots m_slots;

public:
	/// @brief 関数登録
	/// @param slot 関数
	void Connect(Slot slot)
	{
		m_slots.push_back(slot);
	}

	/// @brief 実行
	/// @param args 関数変数
	void Emit(Args...args) const
	{
		for(auto&slot:m_slots)slot(args...);
	}

	void Clear() { m_slots.clear(); };

};


