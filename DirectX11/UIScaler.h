#pragma once
#include <DirectXMath.h>
#include "Cube.h"

/// <summary>
/// �E�B���h�E�T�C�Y�����pUI�̃X�P�[���𒲐�����N���X
/// </summary>
class UIScaler
{
private:
	bool m_isDirty = false;								//Ui�X�P�[�����ύX���ꂽ���ǂ���

	DirectX::XMFLOAT2 m_scale;		//UI�X�P�[��
	DirectX::XMFLOAT2 m_referenceWindowSize = {};			//�E�B���h�E�T�C�Y
	DirectX::XMFLOAT2 m_currentWindowSize = {};			//���݂̃E�B���h�E�T�C�Y

public:
	UIScaler();
	~UIScaler() = default;

	void UpdateScaler();
	const DirectX::XMFLOAT2& GetScale() const { return m_scale; }	//UI�X�P�[�����擾
	const DirectX::XMFLOAT2& GetReferenceWindowSize() const { return m_referenceWindowSize; }	//�Q�ƃE�B���h�E�T�C�Y���擾
	const DirectX::XMFLOAT2 GetCurrentWindowSize() const { return m_currentWindowSize; };

	void SetWindowSize(const DirectX::XMFLOAT2& windowSize);

};


