#include "CameraBase.h"

#include "Material.h"

using namespace DirectX;


CameraBase::CameraBase() :m_fovY(XM_PIDIV4), m_aspect(16.f/9.f), m_nearZ(0.1f), m_farZ(1000.0f)
{
}

const DirectX::XMMATRIX CameraBase::GetViewXM() noexcept
{
	return m_transform.GetWorldToLocalMatrixXM();
}

const DirectX::XMMATRIX CameraBase::GetProjXM(bool isReversed) noexcept
{
	if (isReversed)
	{
		return XMMatrixPerspectiveFovLH(m_fovY, m_aspect, m_farZ, m_nearZ);

	}
	else
	{
		return XMMatrixPerspectiveFovLH(m_fovY, m_aspect, m_nearZ, m_farZ);
	}
}

const DirectX::XMFLOAT4X4 CameraBase::GetViewXMF(bool isTranspose) noexcept
{
	DirectX::XMMATRIX mat = GetViewXM();
	if (isTranspose)
		mat = DirectX::XMMatrixTranspose(mat);
	DirectX::XMFLOAT4X4 fMat;
	DirectX::XMStoreFloat4x4(&fMat, mat);
	return fMat;
}

const DirectX::XMFLOAT4X4 CameraBase::GetProjXMF(bool isTranspose) noexcept
{
	DirectX::XMMATRIX mat = GetProjXM();
	if (isTranspose)
		mat = DirectX::XMMatrixTranspose(mat);
	DirectX::XMFLOAT4X4 fMat;
	DirectX::XMStoreFloat4x4(&fMat, mat);
	return fMat;
}




