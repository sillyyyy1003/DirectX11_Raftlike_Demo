#pragma once
#include "Component.hpp"
#include "Material.h"
#include "IEffect.h"
#include "Primitive.h"


/// @brief 描画機能を持つコンポーネント
class RenderComponent : public Component
{
protected:

	Material* m_pMaterial;
	IEffect* m_pEffect; // Render Effect
	Primitive* m_pModel;

public:
	RenderComponent() : m_pMaterial(nullptr), m_pEffect(nullptr), m_pModel(nullptr) {}
	virtual ~RenderComponent() = default;

	/// @brief マテリアル配置
	void SetMaterial(Material* mat) { m_pMaterial = mat; };

	/// @brief Render Effect配置
	void SetEffect(IEffect* effect) { m_pEffect = effect; };

	/// @brief モデル配置
	void SetModel(Primitive* _model) { m_pModel = _model; };

	Material* GetMaterial() const { return m_pMaterial; }
	IEffect* GetEffect() const { return m_pEffect; }
	Primitive* GetModel() const { return m_pModel; }

	virtual void Render(Transform& transform);

};

/// @brief UIの描画コンポーネント
class UIRenderComponent :
	public RenderComponent
{
public:
	UIRenderComponent() = default;
	virtual ~UIRenderComponent() override= default;

	/// @brief UIの描画
	void Render(Transform& transform) override;

};

