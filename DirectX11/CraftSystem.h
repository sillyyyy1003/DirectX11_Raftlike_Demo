#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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
	std::string m_resultItemName;	// Use Name as key for new item instance
	bool m_isRevealed = false;		// このレシピを習得したかどうか？
};



class Inventory;

/// @brief system to handle crafting items based on recipes
class CraftSystem
{
public:
	typedef std::string ItemName;
	typedef std::string CategoryName;

	/*typedef std::vector<std::unique_ptr<CraftRecipe>> CraftRecipes;*/
	typedef std::vector<CraftRecipe*> CraftRecipes;
	typedef std::unordered_map<CategoryName, CraftRecipes> CraftCategories;
	typedef std::unordered_map<CategoryName, std::string> IconMap;
	typedef std::vector<CategoryName> Categories;

	static CraftSystem& Instance()
	{
		static CraftSystem instance;
		return instance;
	}

	void Init(Inventory* inventory);

	/// @brief check if player has enough ingredients to craft the item
	/// @param recipe 
	/// @return true>> can craft, false>> cannot craft
	bool CheckCraft(CraftRecipe* recipe);

	/// @brief Craft the item based on the recipe
	/// @param recipe 
	bool Craft(CraftRecipe* recipe);

	bool LoadRecipes(const char* filePath);
	bool LoadCraftData(const char* filepath);	// Load craft data to  m_recipes; 

	/// @brief Check if player has enough ingredients to craft the item by item name
	bool CheckCraft(const std::string& itemName);

	/// @brief Craft the item by item name
	bool Craft(const std::string& itemName);

	CraftRecipe* GetRecipe(std::string& recipeName);
	CraftRecipe* GetRecipe(std::string& category, std::string& recipeName);

	IconMap& GetIconMap() { return m_iconMap; }
	Categories& GetCategories() { return m_categories; }

	int GetItemCountInInventory(std::string& itemName) const;
	void UnInit();

	std::string GetIconName(std::string& category);
	CraftRecipes& GetRecipesByCategory(const std::string& categoryName);

	void TryCraftItem(std::string& itemName);

private:
	CraftSystem();
	~CraftSystem() = default;
	Inventory* m_pInventory;	// Inventory to check

	CraftCategories m_recipes;	// load all recipes with categories	
	Categories m_categories;	// 順番でカテゴリーを保存
	IconMap m_iconMap;			// category icon map

	typedef std::unordered_map<ItemName, std::unique_ptr<CraftRecipe>> Recipes;
	Recipes m_pAllRecipes;		// 名前に沿って、全てのレシピを保存する

};

