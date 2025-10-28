#pragma once
#include "Component.hpp"
#include "Material.h"
#include "Model.h"
#include "Transform.h"


/// <summary>
/// GameObject基底クラス
/// </summary>
class GameObject
{

public:
	enum class GameObjectType : uint8_t
	{
		Default,
		Item,
		Player,
		Environment,

	};

	GameObject(GameObjectType type = GameObjectType::Default);
	virtual ~GameObject();

	virtual void Update(float dt);
	virtual void Draw();

	Transform& GetTransform() { return m_transform; };

	/// @brief Add Component to the GameObject
	/// @tparam T 
	/// @param type Component type to add
	/// @param comp Component pointer
	template<typename T>
	void AddComponent(MyComponent::ComponentType type, std::shared_ptr<T> comp)
	{
		m_components[type] = comp;
	}

	/// @brief Get Component from the GameObject
	/// @tparam T Component type to retrieve
	/// @param type Component type
	/// @return Component pointer of type T if found, otherwise nullptr
	template<typename T>
	T* GetComponent(MyComponent::ComponentType type)
	{
		auto it = m_components.find(type);
		if (it != m_components.end())
		{
			return dynamic_cast<T*>(it->second.get()); 
		}

		return nullptr;
	}

	/// @brief Remove component from components
	/// @tparam T template yep
	/// @param type component type to remove
	template<typename T>
	void RemoveComponent(MyComponent::ComponentType type)
	{
		auto it = m_components.find(type);
		if (it != m_components.end())
		{
			m_components.erase(it);
		}
	}

#if defined(_DEBUG) || defined(DEBUG)
	/// @brief Set debug collision scale for visualization
	/// @param scale Scale factor for collision visualization
	void SetDebugCollisionScale(const DirectX::XMFLOAT3& scale) { m_debugCollisionScale = scale; };

	/// @brief Get debug collision scale
	/// @return Current debug collision scale
	const DirectX::XMFLOAT3& GetDebugCollisionScale() const { return m_debugCollisionScale; };
#endif

	void Activate() { m_isActive = true; }
	void DeActivate() { m_isActive = false; }
	bool GetActive() const { return m_isActive; };

	GameObjectType GetGameObjectType()const { return m_objectType; }
	void SetPosition(const DirectX::XMFLOAT3& pos);
protected:

	Transform m_transform;

	typedef std::unordered_map<MyComponent::ComponentType, std::shared_ptr<Component>> Components;
	Components m_components;// 持つコンポーネントのリスト

	bool m_isActive;
	GameObjectType m_objectType;
#if defined(_DEBUG) || defined(DEBUG)
	DirectX::XMFLOAT3 m_debugCollisionScale = { 1,1,1 };
#endif


};


