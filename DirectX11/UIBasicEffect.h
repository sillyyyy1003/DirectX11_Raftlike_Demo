#pragma once
#include "IEffect.h"

/// <summary>
/// 基本のUIレンダリング
/// </summary>
class UIBasicEffect:
	public IEffect
{
protected:

	PixelShader* m_ps;
	VertexShader* m_vs;

	DirectX::XMFLOAT2 m_viewSize;	//現在ウィンドウのサイズ

public:

	UIBasicEffect();
	~UIBasicEffect() override = default;

	void Apply() override;

	void InitPixelShader(PixelShader* _ps) { m_ps = _ps; };
	void InitVertexShader(VertexShader* _vs) { m_vs = _vs; };

	/// @brief 平行投影を使う描画
	/// @param t
	///	@param camera
	void SetWVPMatrix(const Transform& t, CameraBase* camera = nullptr) override;

	/// @brief 基本色の設置
	/// @param mat 
	void SetMaterial(Material* mat) override;

	void ApplyRenderState();

	/// @brief 縦横の比率変更
	/// @param size 
	void SetViewSize(const DirectX::XMFLOAT2& size) { m_viewSize = size; };

	

};

