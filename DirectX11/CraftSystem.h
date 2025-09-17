#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "AudioManager.h"

/// <summary>
/// 表示一个制作配方的?。
/// </summary>
class CraftRecipe
{
public:
	struct Ingredient
	{
		std::string itemName;	// Item name
		int quantity;			// Quantity required
	};

	CraftRecipe() = default;
	~CraftRecipe() = default;

	void Init(const std::string& resultItemName, const std::vector<Ingredient>& ingredients)
	{
		m_resultItemName = resultItemName;
		m_ingredients = ingredients;
	}

	/// @brief 制作物名を取得
	std::string& GetResultItemName() { return m_resultItemName; }

	int GetIngredientsCount() const { return (int)(m_ingredients.size()); };

	std::vector<Ingredient>& GetIngredients() { return m_ingredients; }

	void AddIngredient(const Ingredient& ing) { m_ingredients.push_back(ing); }
	void SetRecipeName(const std::string& recipeName) { m_resultItemName = recipeName; }
private:
	std::vector<Ingredient> m_ingredients;
	std::string m_resultItemName; //Use Name as key for new item instance
};



class Inventory;

/// @brief system to handle crafting items based on recipes
class CraftSystem
{
public:

	CraftSystem();
	~CraftSystem() = default;

	void Init(Inventory* inventory);

	/// @brief check if player has enough ingredients to craft the item
	/// @param recipe 
	/// @return true>> can craft, false>> cannot craft
	bool CheckCraft(CraftRecipe* recipe);

	/// @brief Craft the item based on the recipe
	/// @param recipe 
	bool Craft(CraftRecipe* recipe);

	bool LoadRecipes(const char* filePath);

	/// @brief Check if player has enough ingredients to craft the item by item name
	bool CheckCraft(const std::string& itemName);

	/// @brief Craft the item by item name
	bool Craft(const std::string& itemName);

	CraftRecipe* GetRecipe(std::string& recipeName);

	int GetItemCountInInventory(std::string& itemName) const;



private:

	Inventory* m_pInventory;	// Inventory to check
	//todo: maybe add bag later

	typedef std::string ItemName;
	typedef std::unordered_map<ItemName, std::unique_ptr<CraftRecipe>> Recipes;
	Recipes m_pRecipes;

};

