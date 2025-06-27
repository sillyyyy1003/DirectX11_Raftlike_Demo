#pragma once
#include <DirectXMath.h>

class Transform
{
public:
    Transform() = default;
    Transform(const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& position);
    ~Transform() = default;

    Transform(const Transform&) = default;
    Transform& operator=(const Transform&) = default;

    Transform(Transform&&) = default;
    Transform& operator=(Transform&&) = default;

  
    DirectX::XMFLOAT3 GetScale() const;
    DirectX::XMVECTOR GetScaleXM() const;


    DirectX::XMFLOAT3 GetRotation() const;
    DirectX::XMVECTOR GetRotationXM() const;


    DirectX::XMFLOAT3 GetPosition() const;
    DirectX::XMVECTOR GetPositionXM() const;


    DirectX::XMFLOAT3 GetRightAxis() const;
    DirectX::XMVECTOR GetRightAxisXM() const;

    DirectX::XMFLOAT3 GetUpAxis() const;
    DirectX::XMVECTOR GetUpAxisXM() const;


    DirectX::XMFLOAT3 GetForwardAxis() const;
    DirectX::XMVECTOR GetForwardAxisXM() const;

    DirectX::XMFLOAT4X4 GetLocalToWorldMatrix() const;
    DirectX::XMMATRIX GetLocalToWorldMatrixXM() const;


    DirectX::XMFLOAT4X4 GetWorldToLocalMatrix() const;
    DirectX::XMMATRIX GetWorldToLocalMatrixXM() const;

    void SetScale(const DirectX::XMFLOAT3& scale);
    void SetScale(float x, float y, float z);


    void SetRotation(const DirectX::XMFLOAT3& eulerAnglesInRadian);
    void SetRotation(float x, float y, float z);


    void SetPosition(const DirectX::XMFLOAT3& position);
    void SetPosition(float x, float y, float z);
    void SetPosition(float* pos);

    void Rotate(const DirectX::XMFLOAT3& eulerAnglesInRadian);
    void RotateAxis(const DirectX::XMFLOAT3& axis, float radian);
    void RotateAround(const DirectX::XMFLOAT3& point, const DirectX::XMFLOAT3& axis, float radian);

    void Translate(const DirectX::XMFLOAT3& direction, float magnitude);


    void LookAt(const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up = { 0.0f, 1.0f, 0.0f });

    void LookTo(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& up = { 0.0f, 1.0f, 0.0f });
    static DirectX::XMFLOAT3 GetEulerAnglesFromRotationMatrix(const DirectX::XMFLOAT4X4& rotationMatrix);

private:
    DirectX::XMFLOAT3 m_Scale = { 1.0f, 1.0f, 1.0f };		
    DirectX::XMFLOAT3 m_Rotation = {};								
    DirectX::XMFLOAT3 m_Position = {};								
};

