#pragma once
#include "IEffect.h"

/// <summary>
/// ��{��UI�����_�����O
/// </summary>
class UIBasicEffect:
	public IEffect
{
protected:

	PixelShader* m_ps;
	VertexShader* m_vs;

	DirectX::XMFLOAT2 m_viewSize;	//���݃E�B���h�E�̃T�C�Y

public:

	UIBasicEffect();
	~UIBasicEffect() override = default;

	void Apply() override;

	void InitPixelShader(PixelShader* _ps) { m_ps = _ps; };
	void InitVertexShader(VertexShader* _vs) { m_vs = _vs; };

	/// @brief ���s���e���g���`��
	/// @param t
	///	@param camera
	void SetWVPMatrix(const Transform& t, CameraBase* camera = nullptr) override;

	/// @brief ��{�F�̐ݒu
	/// @param mat 
	void SetMaterial(Material* mat) override;

	void ApplyRenderState();

	/// @brief �c���̔䗦�ύX
	/// @param size 
	void SetViewSize(const DirectX::XMFLOAT2& size) { m_viewSize = size; };

	

};

