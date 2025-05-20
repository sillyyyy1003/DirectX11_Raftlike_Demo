#pragma once
#include "Windows.h"
#include "CameraBase.h"
#include "Transform.h"

/// @brief ��l�̃J����
class FirstPersonCamera : public CameraBase
{
private:

    float mMoveSpeed = 5.0f;    //�ړ����x

    int mState = 0;
    POINT mOldPos = { 0,0 };

    bool isLockPos = false;     // �ʒu�ړ��\�H
    bool isLockAngle = false;   // �p�x�Œ�H

    //�����ʒu
    DirectX::XMFLOAT3 mDefaultPosition = {};

    //UpdateMove�Ŏg����ϐ�
    DirectX::XMFLOAT3 mTargetPosition = {};
    DirectX::XMFLOAT3 mTargetRotation = {};
    DirectX::XMFLOAT3 mDirection = {};
    float mDistance = 0.0f;         //�^�[�Q�b�g�Ƃ̋���
    float mAccumulateTime = 0.f;    // �����̗ݐώ���
    float mDuration = 1.f;
    bool isMoveToTarget = false;    // ��]�^�����邩�ǂ���

    //�J�����h��Ŏg����ϐ�
    bool isShaking = false;
    DirectX::XMFLOAT2 mShakingAmplitude;    //�h��̕�
    /*FastNoiseLite mNoise;*/

public:

    enum CameraKind
    {
        /// No Interaction
        CAM_NONE,
        /// MOVE FREE
        CAM_FREE,
        /// ��ԂŃJ�����ړ�
        CAM_MOVE,
        /// Camera Shaking
        CAM_SHAKE,
    };

    FirstPersonCamera() = default;
    ~FirstPersonCamera() override = default;

    void Init();

    void Update(float dt) override;

    /// @brief �J�����̈ʒu�ݒ�
    void SetPosition(float x, float y, float z);
    void SetPosition(const DirectX::XMFLOAT3& pos);

    /// @brief �J�����̌�����ݒ�
    /// @param pos �J�����̈ʒu
    /// @param target �^�[�Q�b�g�̈ʒu
    /// @param up �����
    void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);


    void LookAt(const DirectX::XMFLOAT3& target);

    /// @brief ��������ɉ����Ċώ@
    /// @param pos �J�����̈ʒu
    /// @param to ����
    /// @param up �����
    void LookTo(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& to, const DirectX::XMFLOAT3& up);

    /// @brief ���s�ړ�
    /// @param d ����
    void Strafe(float d);

    /// @brief ���s
    /// @param d ����
    void Walk(float d);

    /// @brief �O�ɒ��s
    /// @param d ����
    void MoveForward(float d);

    void LookDown();

    /// @brief �㉺�ώ@
    /// @param rad +:UP -:DOWN
    void Pitch(float rad);

    /// @brief ���E�ώ@
    /// @param rad +:LEFT -:RIGHT
    void RotateY(float rad);

    void MoveUpward(float d);

    /// @brief �J�����̊p�x�����b�N����
    void LockCameraAngle(bool isLockAngle) { this->isLockAngle = isLockAngle; };
    bool GetAngleLock() { return isLockAngle; };

    /// @brief �J�����̈ʒu�����b�N����
    void LockCameraPos(bool isLockPos) { this->isLockPos = isLockPos; };
    bool GetPosLock() { return isLockPos; };


    void LockCamera();

    /// @brief 
    /// @param targetPos �w��ʒu
    /// @param targetRot �w��p�x
    /// @param duration �ړ�����
	//void StartMoveToTarget(DirectX::XMFLOAT3 targetPos, DirectX::XMFLOAT3 targetRot, float duration);
    //void BackToDefaultPos();

    //bool IsEndMove();
    //bool IsStartMove();

    /// @brief �J������Ԃ�ݒ肷��
    void SetCameraState(FirstPersonCamera::CameraKind state);

    /// @brief �J�����̗h���ݒ肷��
    /// @param amplitude �h�ꕝ
    /// @param duration �h�ꎞ��(-1�̏ꍇ�͉i��)
    /// @param frequency ���g��
    void SetShake(float amplitude, float frequency, float duration = -1);

    void StopShake();
    bool GetShake() const { return isShaking; };



private:
    void UpdateState();
    void UpdateFlight(DirectX::XMFLOAT2 mouseMove, float dt);

    /// @brief �w��ʒu�E�p�x�Ɉړ�����
    void UpdateMove(float dt);//todo:need to fix the logic problems

    void UpdateShake(float dt);

    /// @brief ����ʒu���A�b�v����
    /// @param dt 
    void ZoomIn(float dt);

    void Shake(float dt);
};

