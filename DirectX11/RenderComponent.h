#pragma once
#include "Component.hpp"
#include "Material.h"
#include "IEffect.h"
#include "Primitive.h"


/// @brief �`��@�\�����R���|�[�l���g
class RenderComponent : public Component
{
protected:

	Material* m_pMaterial;
	IEffect* m_pEffect; // Render Effect
	Primitive* m_pModel;

public:
	RenderComponent() : m_pMaterial(nullptr), m_pEffect(nullptr), m_pModel(nullptr) {}
	virtual ~RenderComponent() = default;

	/// @brief �}�e���A���z�u
	void SetMaterial(Material* mat) { m_pMaterial = mat; };

	/// @brief Render Effect�z�u
	void SetEffect(IEffect* effect) { m_pEffect = effect; };

	/// @brief ���f���z�u
	void SetModel(Primitive* _model) { m_pModel = _model; };

	Material* GetMaterial() const { return m_pMaterial; }
	IEffect* GetEffect() const { return m_pEffect; }
	Primitive* GetModel() const { return m_pModel; }

	virtual void Render(Transform& transform);

};

/// @brief UI�̕`��R���|�[�l���g
class UIRenderComponent :
	public RenderComponent
{
public:
	UIRenderComponent() = default;
	virtual ~UIRenderComponent() override= default;

	/// @brief UI�̕`��
	void Render(Transform& transform) override;

};

