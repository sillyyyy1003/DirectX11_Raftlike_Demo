#include "Player.h"

#include <memory>

Player::Player():
m_pPlayerController(nullptr),
m_pCameraController(nullptr)
{
}

void Player::Init()
{
	//PlayerController初期化
	m_pPlayerController = std::make_unique<PlayerController>(this);
	//CameraController初期化
	m_pCameraController = std::make_shared<CameraController>();
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
	if(GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics))
	{
		PhysicsComponent* physics = GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics);
		//Sync Physics move
		physics->SyncTransformToPhysics(GetTransform());
		
	}

	//=======Camera Update
	m_pCameraController->Update(dt);
	m_pCameraController->UpdateCameraTransform(m_transform);
	m_pPlayerController->m_isControllable = m_pCameraController->GetFirstPersonCamera();

	//=======Input
	m_pPlayerController->Update(dt);

	//GameLogic
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
			this->GetTransform().GetRotation(),
			this->GetTransform().GetPosition(),
		};
		debugRender->Render(t);
	}
#endif	
}

void Player::Strafe(float d)
{
	m_transform.Translate(m_transform.GetRightAxis(), d);
}

void Player::Walk(float d)
{
	DirectX::XMFLOAT3 rightAxis = m_transform.GetRightAxis();
	DirectX::XMVECTOR rightVec = XMLoadFloat3(&rightAxis);
	DirectX::XMVECTOR frontVec = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(rightVec, DirectX::g_XMIdentityR1));
	DirectX::XMFLOAT3 front;
	XMStoreFloat3(&front, frontVec);
	m_transform.Translate(front, d);
}

void Player::MoveForward(float d)
{
	m_transform.Translate(m_transform.GetForwardAxis(), d);
}

void Player::Pitch(float rad)
{
	DirectX::XMFLOAT3 rotation = m_transform.GetRotation();
	// 将绕x轴旋转弧度限制在[-7pi/18, 7pi/18]之间
	rotation.x += rad;
	if (rotation.x > DirectX::XM_PI * 7 / 18)
		rotation.x = DirectX::XM_PI * 7 / 18;
	else if (rotation.x < -DirectX::XM_PI * 7 / 18)
		rotation.x = -DirectX::XM_PI * 7 / 18;

	m_transform.SetRotation(rotation);
}

void Player::RotateY(float rad)
{
	DirectX::XMFLOAT3 rotation = m_transform.GetRotation();
	rotation.y = DirectX::XMScalarModAngle(rotation.y + rad);
	m_transform.SetRotation(rotation);
}

