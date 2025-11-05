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
	/// @param comp Component pointer
	template<typename T>
	void AddComponent(std::shared_ptr<T> comp)
	{
		m_components[T::TYPE] = comp;
	}

	/// @brief Get Component from the GameObject
	/// @tparam T Component type to retrieve
	/// @return Component pointer of type T if found, otherwise nullptr
	template<typename T>
	T* GetComponent()
	{
		auto it = m_components.find(T::TYPE);
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
};


