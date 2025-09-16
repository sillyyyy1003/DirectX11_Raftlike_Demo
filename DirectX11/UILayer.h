#pragma once
#include "UIComponent.h"

class UILayer
{
public:
	UILayer();
	~UILayer() = default;

	void AddComponent(std::shared_ptr<UIComponent> component);
	void AddComponent(UIComponent* component);


	void Update(float deltaTime);
	void Draw();

	bool IsEnable()const { return m_isEnable; }
	void SetEnable(bool enable) { m_isEnable = enable; }

	//======Mouse Events======
	bool HandleMouseDown(float x, float y);
	bool HandleMouseUp(float x, float y);
	void HandleMouseMove(float x, float y);



private:

	std::vector<UIComponent*> m_uiComponents;
	bool m_isEnable = true;

	UIComponent* m_hoveredComponent = nullptr;
	UIComponent* m_pressedComponent = nullptr;
};

