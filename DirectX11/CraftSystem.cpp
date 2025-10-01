#include "CraftSystem.h"
#include <nlohmann/json.hpp>
#include "DebugLog.h"
#include "Inventory.h"
#include "ItemDataBase.h"

CraftSystem::CraftSystem():
	m_pInventory(nullptr)
{
}

void CraftSystem::Init(Inventory* inventory)
{
	m_pInventory = inventory;
}

bool CraftSystem::CheckCraft(CraftRecipe* recipe)
{
	// check if inventory is nullptr
	if (!m_pInventory)
	{
#ifdef _DEBUG
		DebugLog::LogWarning("[CraftSystem] :Inventory is nullptr!");
		return false;
#endif
	}

	// check if player has enough ingredients
	for (const auto& ingredient : recipe->GetIngredients())
	{
		if (!m_pInventory->HasEnoughItem(const_cast<std::string&>(ingredient.itemName), ingredient.quantity))
		{
			DebugLog::LogWarning("[CraftSystem] : {} is not enough!", ingredient.itemName);
			return false;	// if any ingredient is not enough, return false
		}
	}

	// if yes, consume ingredients and add new item to inventory
	return true;
}

bool CraftSystem::Craft(CraftRecipe* recipe)
{
	// Remove all item needed in inventory
	if (!m_pInventory)
	{
#ifdef _DEBUG
		DebugLog::LogWarning("[CraftSystem] :Inventory is nullptr!");
		return false;
#endif
	}

	// Remove ingredients
	for (const auto& ingredient : recipe->GetIngredients())
	{
		m_pInventory->RemoveItem(ingredient.itemName, ingredient.quantity);
	}

	// Add new item to inventory　only create data , not spawn in world
	std::shared_ptr<ItemInstance> newItem = ItemDataBase::Instance().CreateItemInstance(recipe->GetResultItemName().c_str());
	if (m_pInventory->Insert(newItem.get()) == 1) //insert method return inserted num so if ==1, means success
	{
		// play sound

		// ui show success be like get something new

		DebugLog::LogWarning("[CraftSystem] :Successfully craft item to inventory!");
		return true;
	}
	else
	{
		DebugLog::LogWarning("[CraftSystem] :Failed to insert crafted item to inventory! Because inventory is full");
		//todo: maybe make item drop to ground
		return false;
	}

}

bool CraftSystem::LoadRecipes(const char* filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		DebugLog::LogError("[CraftSystem] :Failed to open recipe file:{}", filePath);
		return false;
	}

	nlohmann::json j;
	file >> j;

	if (!j.contains("recipes") || !j["recipes"].is_array())
	{
		DebugLog::LogError("[CraftSystem] :Invalid recipe JSON format: missing 'recipes' array");
		return false;
	}

	for (const auto& recipeJson : j["recipes"])
	{
		auto recipe = std::make_unique<CraftRecipe>();

		// Load recipe name
		std::string resultName = recipeJson.value("recipeName", "");
		recipe->SetRecipeName(resultName);	//recipe name is item name

		// 遍历 ingredient
		if (recipeJson.contains("ingredients") && recipeJson["ingredients"].is_array())
		{
			for (const auto& ing : recipeJson["ingredients"])
			{
				CraftRecipe::Ingredient ingredient;
				ingredient.itemName = ing.value("itemName", "");
				ingredient.quantity = ing.value("quantity", 0);
				recipe->AddIngredient(ingredient);
			}
		}

		// 存到 m_pRecipes
		if (!recipe->GetResultItemName().empty())
		{
			m_pAllRecipes[recipe->GetResultItemName()] = std::move(recipe);
		}
	}

	return true;
}

bool CraftSystem::LoadCraftData(const char* filepath)
{
	std::ifstream file(filepath);
	if (!file.is_open())
	{
		DebugLog::LogError("[CraftSystem] :Failed to open recipe file:{}", filepath);
		return false;
	}

	nlohmann::json j;
	file >> j;

	// check data
	if (!j.contains("Datas") || !j["Datas"].is_object())
	{
		DebugLog::LogError("[CraftSystem] :Invalid recipe JSON format: missing 'Datas' object");
		return false;
	}

	const auto& datas = j["Datas"];
	for (auto itCat = datas.begin(); itCat != datas.end(); ++itCat)
	{
		std::string categoryName = itCat.key();
		const auto& categoryJson = itCat.value();

		m_categories.push_back(categoryName);
		
		// error check name& recipies
		if (!categoryJson.contains("recipes") || !categoryJson["recipes"].is_array())
		{
			DebugLog::LogError("[CraftSystem] :Category '{}' has no valid 'recipes' array", categoryName);
			continue;
		}

		// load recipes from this category
		CraftRecipes& recipes = m_recipes[categoryName];

		for (const auto& recipeJson : categoryJson["recipes"])
		{
			if (!recipeJson.contains("recipeName") || !recipeJson["recipeName"].is_string())
			{
				DebugLog::LogError("[CraftSystem] :Invalid recipe in category '{}'", categoryName);
				continue;
			}

			std::string recipeName = recipeJson["recipeName"];
			auto recipe = std::make_unique<CraftRecipe>();
			recipe->SetRecipeName(recipeName);

			if (recipeJson.contains("ingredients") && recipeJson["ingredients"].is_array())
			{
				for (const auto& ingJson : recipeJson["ingredients"])
				{
					if (!ingJson.contains("itemName") || !ingJson.contains("quantity"))
						continue;

					CraftRecipe::Ingredient ing;
					ing.itemName = ingJson["itemName"].get<std::string>();
					ing.quantity = ingJson["quantity"].get<int>();
					recipe->GetIngredients().push_back(ing);
				}
			}

			recipes.push_back(std::move(recipe));
		}

		if (categoryJson.contains("iconName") && categoryJson["iconName"].is_string())
		{
			m_iconMap[categoryName] = categoryJson["iconName"].get<std::string>();
		}
	}
	return true;
}

bool CraftSystem::CheckCraft(const std::string& itemName)
{
	auto it = m_pAllRecipes.find(itemName);
	if (it == m_pAllRecipes.end())
	{
#ifdef _DEBUG
		DebugLog::Log("[CraftSystem] :There is no such recipe", itemName);
#endif
		return false;
	}
	CraftRecipe* recipe = it->second.get();
	return CheckCraft(recipe);
}

bool CraftSystem::Craft(const std::string& itemName)
{
	auto it = m_pAllRecipes.find(itemName);
	if (it == m_pAllRecipes.end())
	{
#ifdef _DEBUG
		DebugLog::Log("[CraftSystem] :There is no such recipe", itemName);
#endif
		return false;
	}
	CraftRecipe* recipe = it->second.get();
	return Craft(recipe);
}

CraftRecipe* CraftSystem::GetRecipe(std::string& recipeName)
{
	auto it = m_pAllRecipes.find(recipeName);
	if (it == m_pAllRecipes.end())
	{
#ifdef _DEBUG
		DebugLog::Log("[CraftSystem] :There is no such recipe", recipeName);
#endif
		return nullptr;
	}
	return it->second.get();
}

CraftRecipe* CraftSystem::GetRecipe(std::string& category, std::string& recipeName)
{
	// find specific category
	auto itCat = m_recipes.find(category);
	if (itCat == m_recipes.end())
	{
		// if didn't find, DebugLog
		DebugLog::LogError("[CraftSystem] Category not found: {}", category);
		return nullptr;
	}
	// find recipe in this category
	auto& recipes = itCat->second;
	for(const auto& recipe:recipes)
	{
		if(recipe->GetResultItemName()==recipeName)
		{
			return recipe.get();
		}
	}
	DebugLog::LogError("[CraftSystem] Recipe not found: {}", recipeName);
	return nullptr;
}

int CraftSystem::GetItemCountInInventory(std::string& itemName) const
{
	if(m_pInventory)return m_pInventory->GetTotalCount(itemName);

	return 0;
}

void CraftSystem::UnInit()
{

}

std::string CraftSystem::GetIconName(std::string& category)
{
	auto it = m_iconMap.find(category);
	// if find category in icon map,return icon name, else return empty string
	if (it != m_iconMap.end())
	{
		return it->second;
	}
	else
	{
#ifdef _DEBUG
		DebugLog::Log("[CraftSystem] {} Icon not found", category);
#endif
		return "";
	}

}

CraftSystem::CraftRecipes& CraftSystem::GetRecipesByCategory(const std::string& categoryName)
{
	auto result = m_recipes.find(categoryName);
	if (result != m_recipes.end())
	{
		return result->second;
	}
	else
	{
#ifdef _DEBUG
		DebugLog::LogError("[CraftSystem] {} is not found!", categoryName);
#endif
		static CraftRecipes emptyRecipes;
		return emptyRecipes;
	}
}
