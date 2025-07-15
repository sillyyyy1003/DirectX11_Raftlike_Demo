#pragma once
#include "UIElement.h"
#include "UILayer.h"

class UIManager
{
private:

	UIManager() = default;
	~UIManager() = default;

public:
	typedef int Priority;	//Priority is used to determine the rendering order of UI components

	static UIManager& GetInstance()
	{
		static UIManager instance;
		return instance;
	}

	/// @brief Layer追加
	/// @param LayerName Layer name to be added
	/// @param priority 優先度
	void AddUiLayer(const char* LayerName, Priority priority);

	UILayer* GetUILayer(const char* LayerName);

	void Update(float deltaTime);

	void Draw();

	//=========Mouse Event Handlers=========

	bool HandleMouseDown(float x, float y);
	bool HandleMouseUp(float x, float y);
	void HandleMouseMove(float x, float y);

private:

	typedef std::map<Priority, std::shared_ptr<UILayer>> Layers;
	Layers m_layers;	//Map of layers, sorted by priority　数字が多いほど手前に表示される

	typedef std::map<std::string, Priority> LayerNameList;
	LayerNameList m_layerNameList;	//Map of layer names to their priorities


};

