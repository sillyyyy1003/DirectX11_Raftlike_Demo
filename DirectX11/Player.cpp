#include "Player.h"
#include <memory>

#include "GameApp.h"
#include "RenderState.h"

namespace
{
	constexpr float MoveSpeed = 5.f;
	constexpr float RotateLimit = DirectX::XM_PI * 7 / 18;	// 70度, 限制玩家上下视角旋转范围
	constexpr float HungerInitialValue = 200.f;	// 初期の空腹度
	constexpr float HungerStarveSpeed = 1.f;	// 空腹度の減少速度（1秒あたり1ポイント減少）
	constexpr float ThirstInitialValue = 200.f;	// 初期の渇き度
	
}

Player::Player():
	m_pPlayerController(nullptr),
	m_pCameraController(nullptr),
	m_speed(MoveSpeed)
{
}

void Player::Init()
{
	//Player Character (Physics Init)
	m_pPlayerCharacter = std::make_shared<PlayerCharacter>();
	m_pPlayerCharacter->Init();

	//PlayerController初期化
	m_pPlayerController = std::make_unique<PlayerController>(this, m_pPlayerCharacter.get());

	//CameraController初期化
	m_pCameraController = std::make_shared<CameraController>();

	//hunger component初期化→初期値
	m_pHungerComponent = std::make_shared<HungerComponent>(HungerInitialValue);
	m_pHungerComponent->SetStarveSpeed(HungerStarveSpeed); //空腹度の減少速度を設定（1秒あたり1ポイント減少）
	AddComponent(MyComponent::ComponentType::Hunger, m_pHungerComponent);		// HungerComponentをPlayerに追加


}



void Player::Update(float dt)
{
#if defined(_DEBUG) || defined(DEBUG)

	if(ImGui::Begin("Player"))
	{
		float pos[3] = {
			m_transform.GetPosition().x,
			m_transform.GetPosition().y,
			m_transform.GetPosition().z
		};
		ImGui::InputFloat3("Position", pos);
		m_transform.SetPosition(pos);

		float rot[3] = {
			DirectX::XMConvertToDegrees(m_transform.GetRotation().x),
			DirectX::XMConvertToDegrees(m_transform.GetRotation().y),
			DirectX::XMConvertToDegrees(m_transform.GetRotation().z),
		};
		ImGui::InputFloat3("Rotation(Degree)", rot);
		m_transform.SetRotation(DirectX::XMConvertToRadians(rot[0]), DirectX::XMConvertToRadians(rot[1]), DirectX::XMConvertToRadians(rot[2]));

	}

	ImGui::End();
#endif

	//　物理挙動更新
	m_pPlayerCharacter->Update(dt);
	m_pPlayerCharacter->SyncPlayerWorldPosition(m_transform);	//Transformを更新

	//=======Camera Update
	m_pCameraController->Update(dt);
	m_pCameraController->UpdateCameraTransform(m_transform);		//playerのTransformをCameraControllerに反映

	//=======Input
	m_pPlayerController->m_isControllable = m_pCameraController->GetFirstPersonCamera();	//カメラがFirstPersonCameraなら操作可能
	m_pPlayerController->Update(dt);

	//=======Status Update
	m_pHungerComponent->Update(dt);	//空腹度



}

void Player::Draw()
{
#if defined(_DEBUG) || defined(DEBUG)
	//Physical Collider Render
	if(GetComponent<RenderComponent>(MyComponent::ComponentType::DebugRender))
	{
		// Debug Renderスケールを設定（形はCapsule x=z=radius*2 y=height/2）
		// Capsuleの初期高さは2.fなので、y軸のスケールは半分にする
		DirectX::XMFLOAT3 scale = {
			m_pPlayerCharacter->GetDebugDrawRadius() * 2.f,
			m_pPlayerCharacter->GetDebugDrawHeight() * 0.5f,
			m_pPlayerCharacter->GetDebugDrawRadius() * 2.f,
		};
		DirectX::XMFLOAT3 rotation = m_pPlayerCharacter->GetEulerRotation();

		//CharacterVirtual.GetPosition()はキャラクターの足の位置なので、DebugRenderを正確に表示するために、y軸の位置を調整		
		// 足の位置からCapsuleの中心位置に調整(cylinder height+ half sphere radius)
		DirectX::XMFLOAT3 position = m_pPlayerCharacter->GetPosition();
		position.y += 0.5f * (scale.y + m_pPlayerCharacter->GetDebugDrawRadius()); 
		
		RenderComponent* debugRender = GetComponent<RenderComponent>(MyComponent::ComponentType::DebugRender);
		Transform t = {
			scale,
			rotation,
			position
		};

		//Set Transparent draw
		GameApp::SetDepthStencilState(RenderStates::DSSNoDepthWrite);
		debugRender->Render(t);
	}
#endif


}

void Player::Strafe(float dt)
{
	float distance = dt * m_speed;
	m_transform.Translate(m_transform.GetRightAxis(), distance);
}

void Player::Walk(float dt)
{
	float distance = dt * m_speed;
	DirectX::XMFLOAT3 rightAxis = m_transform.GetRightAxis();
	DirectX::XMVECTOR rightVec = XMLoadFloat3(&rightAxis);
	DirectX::XMVECTOR frontVec = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(rightVec, DirectX::g_XMIdentityR1));
	DirectX::XMFLOAT3 front;
	XMStoreFloat3(&front, frontVec);
	m_transform.Translate(front, distance);
}

void Player::MoveForward(float dt)
{
	float distance = dt * m_speed;
	m_transform.Translate(m_transform.GetForwardAxis(), distance);
}

void Player::Pitch(float dt)
{
	float rad = m_speed * dt;
	DirectX::XMFLOAT3 rotation = m_transform.GetRotation();

	rotation.x += rad;
	if (rotation.x > RotateLimit)
		rotation.x = RotateLimit;
	else if (rotation.x < -RotateLimit)
		rotation.x = -RotateLimit;

	m_transform.SetRotation(rotation);
}

void Player::RotateY(float dt)
{
	float rad = m_speed * dt;
	DirectX::XMFLOAT3 rotation = m_transform.GetRotation();
	rotation.y = DirectX::XMScalarModAngle(rotation.y + rad);
	m_transform.SetRotation(rotation);
}

