#pragma once
#include "GameObject.h"
#include "IEffect.h"
#include "Material.h"
#include "MeshBuffer.h"
#include "UIFontSet.h"
#include "UIScaler.h"

namespace
{
	constexpr D2D1_RECT_F DefaultRectSize = { 0,0,100,100 };//UI Default Size (100x100)
}

/// <summary>
/// UI�}�`�̊��N���X
/// </summary>
class UIMesh :public GameObject
{
public:
	UIMesh();
	void SetViewSize(const DirectX::XMFLOAT2& _viewSize);
	void Draw() override;
};


/// @brief UI�`��̊��N���X
class UIElement
{
protected:

	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	std::unique_ptr<UIMesh> m_pUiMesh;					// UI���b�V��

	IDWriteTextFormat* m_pTextFormat;					// �����\��
	ID2D1SolidColorBrush* m_pSolidBrush;				// �����F
	ID2D1RenderTarget* m_pd2dRenderTarget = nullptr;	// �`��R�}���h

	//Transform m_transform;								// �ʒu�E��]�E�g�k
	D2D1_RECT_F m_textRect;								// �����̕`��̈�

	UIScaler* m_pUiScaler = nullptr;					// UI�X�P�[�����O

public:
	UIElement(ID2D1RenderTarget* renderTarget);
	virtual ~UIElement() = default;

	/// @brief �w�i�܂ރ��b�V���̏�����
	/// @param effect �`��pFX
	/// @param material �w�i���b�V���p�}�e���A��
	/// @param model �w�i���b�V��
	/// @param fontSet �t�H���g�t�H�[�}�b�g
	/// @param fontName 
	/// @param uiBrush 
	virtual void Init(IEffect* effect, Material* material,Primitive* model,UIFontSet* fontSet, const char* fontName,UIBrush* uiBrush);

	/// @brief ���������̏�����
	/// @param fontSet Font lib
	/// @param fontName	�t�H���g��
	/// @param uiBrush Brush lib
	virtual void Init(UIFontSet* fontSet, const char* fontName, UIBrush* uiBrush);

	/// @brief Mesh�����̏�����
	/// @param effect �`��pFX
	/// @param material �w�i���b�V���p�}�e���A��
	/// @param model �w�i���b�V��
	virtual void Init(IEffect* effect, Material* material, Primitive* model);

	void SetPosition(const DirectX::XMFLOAT3& pos);
	void SetPosition(float x, float y, float z = 0.1f);
	void SetPosition(const float* pos);

	void SetScale(const DirectX::XMFLOAT3& scale);
	void SetScale(float x, float y, float z = 1.0f);
	void SetScale(const float* scale);

	//=====�`��
	/// @brief ����&���b�V���`��
	/// @param text 
	virtual void DrawUi(const char* text = nullptr);

	/// @brief �����`��
	/// @param text 
	virtual void DrawTextW(const char* text);

	/// @brief UI���b�V���̕`��
	virtual void DrawMesh();

	virtual void Update(float dt);

	
protected:
	/// @brief �����`��ʒu�ݒ�@���S�ʒu����ɕ����̕`��ʒu��ݒ肷��
	void AdjustTextRectPos(float x, float y);

	/// @brief �����`��̈�̃T�C�Y��ݒ肷��@���S�ʒu����ɕ����̕`��ʒu��ݒ肷��
	void AdjustTextRectSize(float width, float height);

	void UpdateScale();
	


};

