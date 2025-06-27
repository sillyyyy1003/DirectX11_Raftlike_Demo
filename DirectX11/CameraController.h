#pragma once
#include <cstdint>
#include <memory>

#include "Windows.h"
#include "CameraBase.h"
#include "Transform.h"

/// @brief カメラの制御を行うクラス
class CameraController
{
public:
	enum class CameraMode:uint8_t
	{
		Free,			// 自由視点
		FirstPerson,	// 一人称視点
		ThirdPerson		// 三人称・ターゲットを追跡
	};

	enum class CameraKind :uint8_t
	{
		/// No Interaction
		CamNone,
		/// MOVE FREE
		CamFree,
	};

private:

	CameraMode m_mode;						// カメラのモード
	CameraKind m_state;
	POINT m_oldPos;							// Cursor操作

	//First Person Camera
	std::shared_ptr<CameraBase> m_pFirstPersonCamera;	// First&Freeカメラ
	std::shared_ptr <CameraBase> m_pThirdPersonCamera;	// Third Personカメラ
	CameraBase* m_pCurrentCamera;

	//First Person Camera param
	DirectX::XMFLOAT3 m_cameraOffset;			//player offset

	//todo:can be removed
	DirectX::XMFLOAT2 m_windowSize;            //Window Size
	float m_moveSpeed;                         //移動速度


public:
	CameraController();
	~CameraController() = default;

	void Update(float dt);

	/// @brief 現在のカメラポインターを取得
	/// @return 
	CameraBase* GetCamera() { return m_pCurrentCamera; };

	/// @brief 外部からカメラのTransformを設定する
	void UpdateCameraTransform(const Transform& transform);

	bool GetFirstPersonCamera();

protected:

	void UpdateThirdPerson(float dt);
	void UpdateFreeCamera(float dt);



};

