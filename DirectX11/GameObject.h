#pragma once
#include "IEffect.h"
#include "Material.h"
#include "Model.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "Transform.h"


/// <summary>
/// GameObject基底クラス
/// </summary>
class GameObject
{
protected:

	Transform m_transform;
	std::unique_ptr<RenderComponent> m_pRenderComponent;						//Game ObjectはRenderComponentを持つ
	std::unordered_map<MyComponent::ComponentType, Component*> m_components;	// GameObjectが持つコンポーネントのリスト
	
public:

	GameObject();
	virtual ~GameObject() = default;

	/// @brief マテリアル配置
	void SetMaterial(Material* mat) { m_pRenderComponent->SetMaterial(mat); };

	/// @brief Render Effect配置
	void SetEffect(IEffect* effect) { m_pRenderComponent->SetEffect(effect); };

	/// @brief モデル配置
	void SetModel(Primitive* model) { m_pRenderComponent->SetModel(model); };

	virtual void Update(float dt);
	virtual void Draw();

	Transform& GetTransform() { return m_transform; };

	/// @brief Add Component to the GameObject
	/// @tparam T 
	/// @param type Component type to add
	/// @param comp Component pointer
	template<typename T>
	void AddComponent(MyComponent::ComponentType type, T* comp)
	{
		m_components[type] = comp;
	}

	/// @brief Get Component from the GameObject
	/// @tparam T Component type to retrieve
	/// @param type Component type
	/// @return omponent pointer of type T if found, otherwise nullptr
	template<typename T>
	T* GetComponent(MyComponent::ComponentType type)
	{
		auto it = m_components.find(type);
		return it != m_components.end() ? it->second : nullptr;
	}

};


