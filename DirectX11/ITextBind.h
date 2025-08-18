#pragma once
#include <functional>
#include <memory>
#include <string>

/// @brief Abstract interface for text binding in UIRendering
class ITextBind
{
public:
	typedef std::function<std::string()> TextProvider;

	virtual void SetTextProvider(TextProvider provider) = 0;
	virtual void SetStaticText(const std::string& text) = 0;

	template<typename T, typename GetterFunc>
	void SetProvider(T* obj, GetterFunc getterFunc)
	{
		SetTextProvider([obj, getterFunc]()
			{
				return getterFunc(obj);
			});
	}

	template<typename T, typename GetterFunc, typename... Args>
	void SetProvider(T* obj, GetterFunc getterFunc, Args... args)
	{
		SetTextProvider([obj, getterFunc, args...]()
			{
				return getterFunc(obj, args...);
			});
	}

	virtual ~ITextBind() = default;
};


class Player; // Forward declaration for Player class
class Inventory; // Forward declaration for Inventory class
namespace UIFormat
{
	/// @brief 現在のプレイヤーの飢餓値をフォーマットして返す
	std::string FormatHunger(Player* player);

	/// @brief 該当するスロットのアイテム数をフォーマットして返す
	std::string FormatItemNumber(Inventory* inventory, int slotIndex);
	
}

