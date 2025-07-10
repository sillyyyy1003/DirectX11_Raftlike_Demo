#include "Player.h"

#include <memory>

namespace
{
	constexpr float moveSpeed = 5.f;
	constexpr float rotateLimit = DirectX::XM_PI * 7 / 18;	// 70度, 限制玩家上下视角旋转范围
	
}

Player::Player():
m_pPlayerController(nullptr),
m_pCameraController(nullptr),
m_moveSpeed(moveSpeed),
m_moveVelocity(0,0,0)
{
}

void Player::Init()
{
	//Player Character (Physics Init)
	m_pPlayerCharacter = std::make_shared<PlayerCharacter>();
	m_pPlayerCharacter->Init();

	//PlayerController初期化
	//m_pPlayerController = std::make_unique<PlayerController>(this);
	m_pPlayerController = std::make_unique<PlayerController>(this,m_pPlayerCharacter.get());

	//CameraController初期化
	m_pCameraController = std::make_shared<CameraController>();

	//hunger component初期化→初期値
	//todo:make this can load from config file
	m_pHungerComponent = std::make_shared<HungerComponent>(200.f);
	AddComponent(MyComponent::ComponentType::HungerManager, m_pHungerComponent.get());

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


	//Physics
	//if(GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics))
	//{
	//	PhysicsComponent* physics = GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics);
	//	//Sync Physics move
	//	physics->PlayerSyncTransformToPhysics(GetTransform());
	//	
	//}
	m_pPlayerCharacter->Update(dt);


	//=======Camera Update
	m_pCameraController->Update(dt);
	m_pCameraController->UpdateCameraTransform(m_transform);
	m_pPlayerController->m_isControllable = m_pCameraController->GetFirstPersonCamera();

	//=======Input
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
		RenderComponent* debugRender = GetComponent<RenderComponent>(MyComponent::ComponentType::DebugRender);
		Transform t = {
			m_debugCollisionScale,
			m_transform.GetRotation(),
			m_pPlayerCharacter->GetPosition(),
		};
		debugRender->Render(t);
	}
#endif

	//Ui Draw
	m_pHungerComponent->Draw();
}

void Player::Strafe(float dt)
{
	float distance = dt * m_moveSpeed;
	m_transform.Translate(m_transform.GetRightAxis(), distance);
}

void Player::Walk(float dt)
{
	float distance = dt * m_moveSpeed;
	DirectX::XMFLOAT3 rightAxis = m_transform.GetRightAxis();
	DirectX::XMVECTOR rightVec = XMLoadFloat3(&rightAxis);
	DirectX::XMVECTOR frontVec = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(rightVec, DirectX::g_XMIdentityR1));
	DirectX::XMFLOAT3 front;
	XMStoreFloat3(&front, frontVec);
	m_transform.Translate(front, distance);
}

void Player::MoveForward(float dt)
{
	float distance = dt * m_moveSpeed;
	m_transform.Translate(m_transform.GetForwardAxis(), distance);
}

void Player::Pitch(float dt)
{
	float rad = m_moveSpeed * dt;
	DirectX::XMFLOAT3 rotation = m_transform.GetRotation();

	rotation.x += rad;
	if (rotation.x > rotateLimit)
		rotation.x = rotateLimit;
	else if (rotation.x < -rotateLimit)
		rotation.x = -rotateLimit;

	m_transform.SetRotation(rotation);
}

void Player::RotateY(float dt)
{
	float rad = m_moveSpeed * dt;
	DirectX::XMFLOAT3 rotation = m_transform.GetRotation();
	rotation.y = DirectX::XMScalarModAngle(rotation.y + rad);
	m_transform.SetRotation(rotation);
}

