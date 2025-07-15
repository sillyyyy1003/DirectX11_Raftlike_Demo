#include "UIManager.h"

#include "DebugLog.h"
#include "GameApp.h"
#include "KInput.h"


void UIManager::AddUiLayer(const char* LayerName, Priority priority)
{
	m_layers[priority] = std::make_shared<UILayer>();
	m_layerNameList[LayerName] = priority;
}

UILayer* UIManager::GetUILayer(const char* LayerName)
{
	// Null check
	auto it =m_layerNameList.find(LayerName);
	if(it== m_layerNameList.end()) 
	{
		DebugLog::Log("[UIManager] Layer {}が見つかりません!", LayerName);
		return nullptr; // Layer not found
	}

	auto layerIt = m_layers.find(it->second);
	if (layerIt == m_layers.end()) return nullptr;

	return layerIt->second.get();
}



void UIManager::Update(float deltaTime)
{
	//======Input Update
	POINT mousePos = GameApp::GetCursorPosition();

	// Mouse Move
	HandleMouseMove((float)mousePos.x, (float)mousePos.y);

	// Mouse Down
	if (KInput::IsKeyPress(VK_LBUTTON))
		HandleMouseDown((float)mousePos.x, (float)mousePos.y);
	// Mouse Up
	if(KInput::IsKeyRelease(VK_LBUTTON))
		HandleMouseUp((float)mousePos.x, (float)mousePos.y);


	//=======UI Update
	for(auto& [priority, layer] : m_layers)
	{
		layer->Update(deltaTime);
	}

}

void UIManager::Draw()
{
	for(auto& [priority, layer] : m_layers)
	{
		layer->Draw();
	}
}

bool UIManager::HandleMouseDown(float x, float y)
{
	for(auto& [priority, layer] : m_layers)
	{
		if(layer->HandleMouseDown(x, y)) 
		{
			return true; // If any layer handles the mouse down event, return true
		}
	}
	return false;
}

bool UIManager::HandleMouseUp(float x, float y)
{
	for(auto& [priority, layer] : m_layers)
	{
		if(layer->HandleMouseUp(x, y)) 
		{
			return true; // If any layer handles the mouse up event, return true
		}
	}
	return false;
}

void UIManager::HandleMouseMove(float x, float y)
{
	for(auto& [priority, layer] : m_layers)
	{
		layer->HandleMouseMove(x, y); // Call HandleMouseMove on each layer
	}
	
}


