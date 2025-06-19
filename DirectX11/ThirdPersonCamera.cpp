#include "ThirdPersonCamera.h"

ThirdPersonCamera::~ThirdPersonCamera()
{
}

DirectX::XMFLOAT3 ThirdPersonCamera::GetTargetPosition() const
{
    return m_target;
}

float ThirdPersonCamera::GetDistance() const
{
    return m_distance;
}

void ThirdPersonCamera::RotateX(float rad)
{
	DirectX::XMFLOAT3 rotation = m_transform.GetRotation();
    // 将绕x轴旋转弧度限制在[0, pi/3]之间
    rotation.x += rad;
    if (rotation.x < 0.0f)
        rotation.x = 0.0f;
    else if (rotation.x>DirectX::XM_PI / 3)
        rotation.x = DirectX::XM_PI / 3;

    m_transform.SetRotation(rotation);
    m_transform.SetPosition(m_target);
    m_transform.Translate(m_transform.GetForwardAxis(), -m_distance);
}

void ThirdPersonCamera::RotateY(float rad)
{
	DirectX::XMFLOAT3 rotation = m_transform.GetRotation();
    rotation.y = DirectX::XMScalarModAngle(rotation.y + rad);

    m_transform.SetRotation(rotation);
    m_transform.SetPosition(m_target);
    m_transform.Translate(m_transform.GetForwardAxis(), -m_distance);
}

void ThirdPersonCamera::Approach(float dist)
{
    m_distance += dist;
    // 限制距离在[m_MinDist, m_maxDist]之间
    if (m_distance < m_minDist)
        m_distance = m_minDist;
    else if (m_distance > m_maxDist)
        m_distance = m_maxDist;

    m_transform.SetPosition(m_target);
    m_transform.Translate(m_transform.GetForwardAxis(), -m_distance);
}

void ThirdPersonCamera::SetRotationX(float rad)
{
	DirectX::XMFLOAT3 rotation = m_transform.GetRotation();
    // 将绕x轴旋转弧度限制在[0, pi/3]之间
    rotation.x = rad;
    if (rotation.x < 0.0f)
        rotation.x = 0.0f;
    else if (rotation.x > DirectX::XM_PI / 3)
        rotation.x = DirectX::XM_PI / 3;

    m_transform.SetRotation(rotation);
    m_transform.SetPosition(m_target);
    m_transform.Translate(m_transform.GetForwardAxis(), -m_distance);
}

void ThirdPersonCamera::SetRotationY(float rad)
{
	DirectX::XMFLOAT3 rotation = m_transform.GetRotation();
    rotation.y = DirectX::XMScalarModAngle(rad);
    m_transform.SetRotation(rotation);
    m_transform.SetPosition(m_target);
    m_transform.Translate(m_transform.GetForwardAxis(), -m_distance);
}

void ThirdPersonCamera::SetTarget(const DirectX::XMFLOAT3& target)
{
    m_target = target;
}

void ThirdPersonCamera::SetDistance(float dist)
{
    m_distance = dist;
}

void ThirdPersonCamera::SetDistanceMinMax(float minDist, float maxDist)
{
    m_minDist = minDist;
    m_maxDist = maxDist;
}