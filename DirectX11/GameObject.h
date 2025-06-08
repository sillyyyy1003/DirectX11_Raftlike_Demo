#pragma once
#include "Cube.h"
#include "IEffect.h"
#include "Material.h"
#include "Model.h"
#include "Transform.h"

/// <summary>
/// GameObject基底クラス
/// </summary>
class GameObject
{
protected:

	Transform m_transform;

	Material* m_pMaterial;
	BasicEffect* m_pEffect;
	//Cube* m_pModel;
	Model* m_pModel;

public:
	/// @brief マテリアル配置
	void SetMaterial(Material* mat) { m_pMaterial = mat; };

	/// @brief Render Effect配置
	void SetEffect(BasicEffect* effect) { m_pEffect = effect; };

	/// @brief モデル配置
	//void SetModel(Cube* _cube) { m_pModel = _cube; };
	void SetModel(Model* _model){m_pModel=_model;};

	GameObject();
	virtual ~GameObject() = default;
	virtual void Update(float dt);
	virtual void Draw();
};

