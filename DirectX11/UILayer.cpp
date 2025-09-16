#include "UILayer.h"

UILayer::UILayer()
{
}

void UILayer::AddComponent(std::shared_ptr<UIComponent> component)
{
	//m_uiComponents.push_back(component);
}

void UILayer::AddComponent(UIComponent* component)
{
	m_uiComponents.push_back(component);
}

void UILayer::Update(float deltaTime)
{
	for(const auto& component : m_uiComponents)
	{
		if (component->IsActive())
		{
			component->Update(deltaTime);
		}
	}
}

void UILayer::Draw()
{
	for(const auto& component : m_uiComponents)
	{
		if (component->IsActive())
		{
			component->Draw();
		}
	}
}

bool UILayer::HandleMouseDown(float x, float y)
{
	
	for (const auto& component : m_uiComponents)
	{
		if (component->IsActive())
		{
			if (UIComponent* hit = component->HitTest(x, y))
			{
				component->OnMouseDown(x, y);
				//Record the component that handled the mouse down event
				m_pressedComponent = hit;
				// Mouse down handled by this component
				return true;
			}
		}
	}
	return false; // No component handled the mouse down event
}

bool UILayer::HandleMouseUp(float x, float y)
{
	if (m_pressedComponent && m_pressedComponent->IsActive())
	{
		m_pressedComponent->OnMouseUp(x, y);
		if (m_pressedComponent->HitTest(x, y) == m_pressedComponent)
			m_pressedComponent->OnClick(x, y);

		m_pressedComponent = nullptr;
		return true;
	}
	return false;
}

void UILayer::HandleMouseMove(float x, float y)
{
	/*
	UIComponent* hoveredComponent = nullptr;
	for(const auto&component:m_uiComponents)
	{
		if (component->IsActive() && component->HitTest(x, y))
		{
			hoveredComponent = component;
			break;
		}
	}

	if(hoveredComponent!=m_hoveredComponent)
	{
		if (m_hoveredComponent)m_hoveredComponent->OnMouseExit();
		if (hoveredComponent) hoveredComponent->OnMouseEnter();
		m_hoveredComponent = hoveredComponent;
	}*/
	UIComponent* hoveredComponent = nullptr;
	for (const auto& component : m_uiComponents)
	{
		if (component->IsActive())
		{
			if (UIComponent* hit = component->HitTest(x, y))
			{
				hoveredComponent = hit;
				break;
			}
		}
	}

	if (hoveredComponent != m_hoveredComponent)
	{
		if (m_hoveredComponent)m_hoveredComponent->OnMouseExit();
		if (hoveredComponent) hoveredComponent->OnMouseEnter();
		m_hoveredComponent = hoveredComponent;
	}

}
