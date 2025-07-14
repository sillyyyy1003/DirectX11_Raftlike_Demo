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

	virtual ~ITextBind() = default;
};


class Player; // Forward declaration for Player class
namespace UIFormat
{
	// Helper function to create a text provider from a static string
	std::string FormatHunger(Player* player);
	
}

