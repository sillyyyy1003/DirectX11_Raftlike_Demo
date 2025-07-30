#pragma once
#include "IEffect.h"

class UIGlossEffect:
	public IEffect
{
public:
	UIGlossEffect();
	~UIGlossEffect() override = default;

	void Apply() override;

	void InitPixelShader(PixelShader* _ps) { m_ps = _ps; };
	void InitVertexShader(VertexShader* _vs) { m_vs = _vs; };

	/// @brief 平行投影を使う描画
	/// @param t
	///	@param camera
	void SetWVPMatrixCB(const Transform& t, CameraBase* camera = nullptr) override;

	/// @brief 基本色の設置
	/// @param mat 
	void SetMaterial(Material* mat) override;

	void ApplyRenderState();

	/// @brief 縦横の比率変更
	/// @param size 
	void SetViewSize(const DirectX::XMFLOAT2& size);;

private:
	PixelShader* m_ps;
	VertexShader* m_vs;

	DirectX::XMFLOAT2 m_viewSize;	//現在ウィンドウのサイズ


};

