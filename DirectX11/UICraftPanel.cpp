#include "UICraftPanel.h"
#include "CraftSystem.h"
#include "ItemDataBase.h"
#include "TextureManager.h"
#include "d3dUtil.h"

namespace
{
	static constexpr int IngredientsSize = 5;

}

UICraftPanel::UICraftPanel() :
	m_position(0,0,0),
	m_size(0,0),
	m_backgroundRender(nullptr),
	m_craftIcon(nullptr),
	m_itemNameUI(nullptr),
	m_descriptionTextUI(nullptr),
	m_requiresTextUI(nullptr),
	m_craftButton(nullptr),
	m_ingredients(IngredientsSize),
	m_craftSystem(nullptr),
	m_iconSize(64.f),                // all icon size
	m_buttonHeight(50.f),            // craft button height
	m_buttonWidth(128.f),            // craft button width
	m_horizontalPadding(10.f),       // Horizontal distance between uis & edge
	m_verticalPadding(10.f),         // Vertical distance between uis & edge
	m_horizontalSpacing(10.f),       // Horizontal spacing between ui
	m_verticalSpacing(10.f),         // Vertical spacing between ui
	m_rectWidth(200.f),
	m_titleTextUIHeight(54.f),
	m_descriptionHeight(80.f),
	m_requireTextHeight(31.f)
{
}

void UICraftPanel::InitRender(Material* backgroundMaterial, Material* buttonMaterial, Material* iconBackGroundMaterial, Material* iconsMaterial, IEffect* effect,	Primitive* primitive)
{
	m_backgroundRender = std::make_unique<UIRender>();
	m_backgroundRender->Init(backgroundMaterial, effect, primitive);

	m_craftIcon = std::make_unique<UIRender>();
	m_craftIcon->Init(iconsMaterial, effect, primitive);

	m_craftButton= std::make_unique<UIButton>();
	m_craftButton->Init(effect, buttonMaterial, primitive);

	m_craftIconBackground = std::make_unique<UIRender>();
	m_craftIconBackground->Init(iconBackGroundMaterial, effect, primitive);

	for(auto& uiIngredient:m_ingredients)
	{
		uiIngredient.ingredientIconUI = std::make_unique<UIRender>();
		uiIngredient.ingredientIconUI->Init(iconsMaterial, effect, primitive);
		uiIngredient.ingredientIconBackgroundUI = std::make_unique<UIRender>();
		uiIngredient.ingredientIconBackgroundUI->Init(iconBackGroundMaterial, effect, primitive);
	}
}

void UICraftPanel::InitSizeAndPos(const DirectX::XMFLOAT2& size, const DirectX::XMFLOAT3& pos)
{
	m_position = pos;
	m_size = size;
	// All ui based on m_position(Anchor:left top)
	//=======Background
	DirectX::XMFLOAT2 backgroundSize = m_size;
	m_backgroundRender->GetTransform().SetScale(backgroundSize.x, backgroundSize.y, 1.0);	 // Set Scale 
	DirectX::XMFLOAT3 bgPosition = { m_position.x + backgroundSize.x / 2.f,m_position.y - backgroundSize.y/2.f,m_position.z };
	m_backgroundRender->GetTransform().SetPosition(bgPosition);	  // Set position

	//=======title icon
	
	DirectX::XMFLOAT3 titleIconPos = {
		m_position.x + m_horizontalPadding+ m_iconSize / 2.f,
		m_position.y - m_verticalPadding- m_iconSize / 2.f,
		m_position.z
	};

	m_craftIcon->GetTransform().SetScale(m_iconSize, m_iconSize, 1.f);
	m_craftIcon->GetTransform().SetPosition(titleIconPos);
	m_craftIconBackground->GetTransform().SetScale(m_iconSize, m_iconSize, 1.f);
	m_craftIconBackground->GetTransform().SetPosition(titleIconPos);

	//========item Name UI text
	DirectX::XMFLOAT3 titleTextScale = {
		m_rectWidth,
		m_titleTextUIHeight,
		1.f
	};

	DirectX::XMFLOAT3 titleTextPos = {
		m_position.x + m_horizontalSpacing + m_iconSize + 10.f + titleTextScale.x / 2.f,
		titleIconPos.y,
		1.f
	};
	
	m_itemNameUI->SetScale(titleTextScale);
	m_itemNameUI->SetPosition(titleTextPos);

	//=========description text
	DirectX::XMFLOAT3 descriptionTextScale = {
		backgroundSize.x - m_horizontalPadding - m_horizontalPadding,//left& right offset
		m_descriptionHeight,
		1.f
	};
	DirectX::XMFLOAT3 descriptionTextPos = {
		m_position.x + m_horizontalPadding + descriptionTextScale.x / 2.f,
		titleIconPos.y - m_iconSize/2.f - m_verticalSpacing -descriptionTextScale.y/2.f,
		1.0f
	};
	m_descriptionTextUI->SetScale(descriptionTextScale);
	m_descriptionTextUI->SetPosition(descriptionTextPos);

	//===========Requires text
	DirectX::XMFLOAT3 requireTextScale = {
	m_rectWidth,
	m_requireTextHeight,
	1.f};
	DirectX::XMFLOAT3 requiresPos = {
		m_position.x + m_horizontalPadding + requireTextScale.x / 2.f,
		 descriptionTextPos.y - descriptionTextScale.y / 2.f - m_verticalSpacing-requireTextScale.y/2.f,
		1.f
	};
	m_requiresTextUI->SetScale(requireTextScale);
	m_requiresTextUI->SetPosition(requiresPos);

	//===========CraftButton
	DirectX::XMFLOAT3 buttonPos = {
		m_position.x + m_size.x - 10.f - m_buttonWidth / 2.f,
		requiresPos.y - requireTextScale.y / 2.f + m_buttonHeight / 2.f,
		m_position.z + 0.1f
	};
	m_craftButton->SetButton(buttonPos, m_buttonWidth, m_buttonHeight);

	//===========UIIngredient
	float ingredientPosX = titleIconPos.x;
	float ingredientPosY = requiresPos.y - requireTextScale.y / 2.f - m_verticalSpacing -m_iconSize/2.f;
	DirectX::XMFLOAT3 ingredientIconScale = { m_iconSize,m_iconSize,1.f };
	for(int i=0;i<m_ingredients.size();i++)
	{
		// Set Icon
		m_ingredients[i].ingredientIconUI->GetTransform().SetScale(ingredientIconScale);
		DirectX::XMFLOAT3 iconPos = {
			ingredientPosX,
			ingredientPosY - i * (m_iconSize + m_verticalSpacing),
			m_position.z + 0.1f,
		};

		m_ingredients[i].ingredientIconUI->GetTransform().SetPosition(iconPos);
		m_ingredients[i].ingredientIconBackgroundUI->GetTransform().SetScale(ingredientIconScale);
		m_ingredients[i].ingredientIconBackgroundUI->GetTransform().SetPosition(iconPos);

		// set ingredient name
		DirectX::XMFLOAT3 ingredientNameUiScale = {
		m_rectWidth,
		m_iconSize,
		1.f
		};
		DirectX::XMFLOAT3 ingredientTextPos = {
			ingredientPosX + m_iconSize / 2.f + m_horizontalSpacing+ ingredientNameUiScale.x/2.f,
			iconPos.y,
			1.f
		};
	
		m_ingredients[i].ingredientNameUI->SetScale(ingredientNameUiScale);
		m_ingredients[i].ingredientNameUI->SetPosition(ingredientTextPos);

		// set require number text
		DirectX::XMFLOAT3 requiredNumTexScale = {
		m_rectWidth,
		m_iconSize,
		1.f
		};
		DirectX::XMFLOAT3 requiredNumTexPos = {
			m_position.x + m_size.x - m_horizontalPadding - requiredNumTexScale.x / 2.f,
			iconPos.y,
			1.f
		};
		
		m_ingredients[i].requiredNumberTextUI->SetScale(requiredNumTexScale);
		m_ingredients[i].requiredNumberTextUI->SetPosition(requiredNumTexPos);
	}
}

void UICraftPanel::InitFonts(UIFontSet* fontSet, UIBrush* brush, const char* itemNameFont, const char* descriptionFont,
	const char* requiresFont, const char* ingredientNameFont, const char* numberFont)
{
	m_itemNameUI = std::make_unique<UIText>();
	m_itemNameUI->Init(fontSet, itemNameFont, brush);
	m_itemNameUI->SetStaticText("itemName");
	m_itemNameUI->SetParagraphAlignment(UIText::ParagraphAlign::Middle);
	

	m_descriptionTextUI = std::make_unique<UIText>();
	m_descriptionTextUI->Init(fontSet, descriptionFont, brush);
	m_descriptionTextUI->SetStaticText("description");

	m_requiresTextUI = std::make_unique<UIText>();
	m_requiresTextUI->Init(fontSet, requiresFont, brush);
	m_requiresTextUI->SetStaticText("REQUIRES");
	m_requiresTextUI->SetParagraphAlignment(UIText::ParagraphAlign::Middle);

	for(auto& ingredientUi:m_ingredients)
	{
		ingredientUi.ingredientNameUI = std::make_unique<UIText>();
		ingredientUi.ingredientNameUI->Init(fontSet, ingredientNameFont, brush);
		ingredientUi.ingredientNameUI->SetParagraphAlignment(UIText::ParagraphAlign::Middle);
		ingredientUi.ingredientNameUI->SetStaticText("IngredientName");

		ingredientUi.requiredNumberTextUI = std::make_unique<UIText>();
		ingredientUi.requiredNumberTextUI->Init(fontSet, numberFont, brush);
		ingredientUi.requiredNumberTextUI->SetTextAlignment(UIText::TextAlign::Right);	 // set text alignment right
		ingredientUi.requiredNumberTextUI->SetParagraphAlignment(UIText::ParagraphAlign::Middle);
		ingredientUi.requiredNumberTextUI->SetStaticText("N/N");
	}
}

void UICraftPanel::LoadPanelConfig(nlohmann::json& j, const char* panelName)
{
	// Check if json contains the requested panel data
	if (!j.contains(panelName))
	{
		DebugLog::Log("UICraftPanel::LoadPanelConfig -> Panel data not found in json");
		return;
	}

	auto& ui = j[panelName];

	//================= Panel Size & Position =================
	if (ui.contains("Position"))
	{
		auto pos = ui["Position"];
		if (pos.is_array() && pos.size() >= 3)
		{
			m_position = {
				pos[0].get<float>(),
				pos[1].get<float>(),
				pos[2].get<float>()
			};
		}
	}

	if (ui.contains("Size"))
	{
		auto size = ui["Size"];
		if (size.is_array() && size.size() >= 2)
		{
			m_size = {
				size[0].get<float>(),
				size[1].get<float>()
			};
		}
	}

	//================= Config Parameters =================
	if (ui.contains("IconSize"))             m_iconSize = ui["IconSize"].get<float>();
	if (ui.contains("ButtonWidth"))          m_buttonWidth = ui["ButtonWidth"].get<float>();
	if (ui.contains("ButtonHeight"))         m_buttonHeight = ui["ButtonHeight"].get<float>();
	if (ui.contains("HorizontalPadding"))    m_horizontalPadding = ui["HorizontalPadding"].get<float>();
	if (ui.contains("VerticalPadding"))      m_verticalPadding = ui["VerticalPadding"].get<float>();
	if (ui.contains("HorizontalSpacing"))    m_horizontalSpacing = ui["HorizontalSpacing"].get<float>();
	if (ui.contains("VerticalSpacing"))      m_verticalSpacing = ui["VerticalSpacing"].get<float>();

	if (ui.contains("RectWidth"))            m_rectWidth = ui["RectWidth"].get<float>();
	if (ui.contains("TitleTextUIHeight"))    m_titleTextUIHeight = ui["TitleTextUIHeight"].get<float>();
	if (ui.contains("DescriptionHeight"))    m_descriptionHeight = ui["DescriptionHeight"].get<float>();
	if (ui.contains("RequireTextHeight"))    m_requireTextHeight = ui["RequireTextHeight"].get<float>();

	D2D1_COLOR_F color = { 1,1,1,1 };
	if (ui.contains("TextColor"))			color = {
	ui["TextColor"][0].get<float>(),ui["TextColor"][1].get<float>(),ui["TextColor"][2].get<float>(),ui["TextColor"][3].get<float>() };
	// set text color
	m_itemNameUI->SetTextColor(color);
	m_descriptionTextUI->SetTextColor(color);
	m_requiresTextUI->SetTextColor(color);

	for (auto& ingredientUi : m_ingredients)
	{

		ingredientUi.ingredientNameUI->SetTextColor(color);
		ingredientUi.requiredNumberTextUI->SetTextColor(color);
	
	}

	//================= Apply loaded values to UI =================
	InitSizeAndPos(m_size, m_position);

	DebugLog::Log("UICraftPanel::LoadPanelConfig -> Config loaded successfully");
}

void UICraftPanel::UpdatePanelInfo(std::string& recipeName)
{
	CraftRecipe* recipe = m_craftSystem->GetRecipe(recipeName);

	std::string itemName = recipe->GetResultItemName();
	const Item* targetItem = ItemDataBase::Instance().GetItem(itemName.c_str()).get();

#ifdef _DEBUG
	if (!targetItem)
	{
		DebugLog::LogError("[UICraftPanel] Target item {} doesn't exist!", itemName);
		return;
	}
#endif

	// Set Title
	std::string title = targetItem->GetName();
	m_itemNameUI->SetStaticText(title);
	// Set description
	std::string description = targetItem->GetItemDescription();
	m_descriptionTextUI->SetStaticText(description);
	// Set Texture
	std::string iconTextureName = targetItem->GetIconTextureName();
	m_pIconTexture = TextureManager::Instance().GetTexture(iconTextureName);
	
	std::vector<CraftRecipe::Ingredient> ingredients = recipe->GetIngredients();
	// lazy update icon to be shown
	m_currentRecipeIngredientCount = recipe->GetIngredientsCount();
	for (int i = 0; i < m_currentRecipeIngredientCount; i++)
	{
		//=========set texture

		std::string ingredientName = ingredients[i].itemName;
		std::string ingredientTextureName = ItemDataBase::Instance().GetItem(ingredientName.c_str())->GetIconTextureName();
		m_ingredients[i].ingredientIconTexture = TextureManager::Instance().GetTexture(ingredientTextureName);
		//=========set ingredient text
		m_ingredients[i].ingredientNameUI->SetStaticText(ingredientName);
		//=========set required num text
		std::string requiredNumStr = std::to_string(m_craftSystem->GetItemCountInInventory(ingredientName)) + "/" + std::to_string(ingredients[i].quantity);
		m_ingredients[i].requiredNumberTextUI->SetStaticText(requiredNumStr);

	}

	// Update BackGroundSize
	float totalHeight = m_verticalPadding + m_iconSize + m_verticalSpacing + m_descriptionHeight + m_verticalSpacing + m_requireTextHeight + m_verticalSpacing;
	for (int i = 0; i < m_currentRecipeIngredientCount; i++)
	{
		totalHeight += (m_iconSize + m_verticalSpacing);
	}

	m_size.y = totalHeight;
	m_backgroundRender->GetTransform().SetScale({ m_size.x,m_size.y,1.f });
	m_backgroundRender->GetTransform().SetPosition({
		m_position.x + m_size.x / 2.f,
		m_position.y - m_size.y / 2.f,
		m_position.z
	});
}

void UICraftPanel::Draw()
{
	if (!IsActive())return;
	m_backgroundRender->Draw();
	m_craftButton->Draw();
	// Update craft icon because all icons share the same material
	m_craftIconBackground->Draw();
	m_craftIcon->GetRenderComponent()->GetMaterial()->SetTexture(Material::Albedo, m_pIconTexture);
	m_craftIcon->Draw();


	// lazy update 
	for (int i = 0; i < m_currentRecipeIngredientCount; i++)
	{
		m_ingredients[i].ingredientIconBackgroundUI->Draw();
		// Set icon texture
		m_ingredients[i].ingredientIconUI->GetRenderComponent()->GetMaterial()->SetTexture(Material::Albedo, m_ingredients[i].ingredientIconTexture);
		m_ingredients[i].ingredientIconUI->Draw();

		// text
		m_ingredients[i].ingredientNameUI->Draw();
		m_ingredients[i].requiredNumberTextUI->Draw();
	}

	//text
	m_descriptionTextUI->Draw();
	m_itemNameUI->Draw();
	m_requiresTextUI->Draw();
}
