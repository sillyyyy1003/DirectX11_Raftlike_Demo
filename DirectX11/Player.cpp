#include "Player.h"
#include <memory>
#include "DebugLog.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
#include "GameApp.h"
#include "RenderState.h"

namespace
{
	static constexpr float RotateLimit = DirectX::XM_PI * 7 / 18;	// 70度, 限制玩家上下视角旋转范围

	static constexpr float HungerInitialValue = 200.f;	// 初期の空腹度
	static constexpr float HungerSpeed = 0.15f;	// 空腹度の減少速度（1秒あたり1ポイント減少）(1day =900s  make 1.5day player get starve speed=200/(1350)
	static constexpr float HungerThreshold = 40.f;	// 空腹度のしきい値（1/4以下で空腹状態になる）

	static constexpr float ThirstInitialValue = 200.f;	// 初期の渇き度
	static constexpr float ThirstSpeed = 0.22f;	// 渇き度の減少速度（1秒あたり1.5ポイント減少）(1day =900s  make 1day player get starve speed=200/*(900)
	static constexpr float ThirstThreshold = 40.f;	// 渇き度のしきい値（1/4以下で空腹状態になる）

	static constexpr int PlayerInventorySize = 12;
	static constexpr float PlayerDefaultHealth = 100.f;

	static constexpr float PlayerJumpSpeed = 4.f;
	static constexpr float PlayerMoveSpeed = 4.f;

	static constexpr DirectX::XMFLOAT3 DefaultCameraPos = { 0,10,-10 };	// Default Camera Position
	static constexpr DirectX::XMFLOAT3 DefaultCameraTarget = { 0,0,0 };	// Default Camera Target Position
	static constexpr DirectX::XMFLOAT3 PlayerEyeHeight = { 0,2,0 };		// Default eye pos
	static constexpr float NegativeSpeedEffector = 0.5f;	// 負のステータスの影響を受ける速度の倍率（0.5倍）

	enum ItemState :int
	{
		Active = 0,         // Spawned and waiting to be picked up
		InActive = 1,       // Can be spawned
		WaitToRecycle = 2,  // Marked for recycling

	};

}

Player::Player():
	GameObject(GameObjectType::Player),
	m_pPlayerController(nullptr),
	m_pCameraController(nullptr),
	m_moveSpeed(PlayerMoveSpeed),
	m_jumpSpeed(PlayerJumpSpeed),
	m_negativeStatusScale(NegativeSpeedEffector)
{
}

Player::~Player()
{
	if (m_pPlayerController)m_pPlayerController.reset();
	if (m_pCameraController)m_pCameraController.reset();
	if (m_pPlayerCharacter)m_pPlayerCharacter.reset();
	if (m_pPlayerEntity)m_pPlayerEntity.reset();
	if (m_pHungerComponent)m_pHungerComponent.reset();
	if(m_pThirstComponent)m_pThirstComponent.reset();
	if (m_pInventory)m_pInventory.reset();

	
}


bool Player::Init(const char* filePath)
{
	if(filePath == nullptr)
	{
		m_pPlayerEntity = std::make_shared<PlayerEntity>(PlayerDefaultHealth);
		AddComponent(MyComponent::ComponentType::LivingEntity, m_pPlayerEntity);

		//Player Character (Physics Init)
		m_pPlayerCharacter = std::make_shared<PlayerCharacter>();
		m_pPlayerCharacter->Init();

		//PlayerController初期化
		m_pPlayerController = std::make_unique<PlayerController>(this, m_pPlayerCharacter.get());

		//CameraController初期化
		m_pCameraController = std::make_shared<CameraController>();

		//hunger component初期化→初期値
		m_pHungerComponent = std::make_shared<HungerComponent>(HungerInitialValue);
		m_pHungerComponent->SetStarveSpeed(HungerSpeed); //空腹度の減少速度を設定（1.5day）
		AddComponent(MyComponent::ComponentType::Hunger, m_pHungerComponent);		// HungerComponentをPlayerに追加

		m_pThirstComponent = std::make_shared<ThirstComponent>(ThirstInitialValue);
		m_pThirstComponent->SetThirstSpeed(ThirstSpeed); //渇き度の減少速度を設定（1day）
		AddComponent(MyComponent::ComponentType::Thirst, m_pThirstComponent);		// ThirstComponentをPlayerに追加

		m_pInventory = std::make_shared<Inventory>(PlayerInventorySize);		// assume max slot
		return true; // No JSON file provided, using default values
	}


	std::ifstream ifs(filePath);
	if (!ifs.is_open())
	{
		DebugLog::LogError("[Player] Failed to open JSON file: {}", filePath);
		return false;
	}

	json j;
	try
	{
		ifs >> j;
	}
	catch (const std::exception& e)
	{
		DebugLog::LogError("[Player] Failed to parse JSON: {}", e.what());
		return false;
	}

	// Player Entity
	{
		float health = j.contains("PlayerEntity")
		   ? j["PlayerEntity"].value("Health", PlayerDefaultHealth)
		   : PlayerDefaultHealth;
		m_pPlayerEntity = std::make_shared<PlayerEntity>(health);
		AddComponent(MyComponent::ComponentType::LivingEntity, m_pPlayerEntity);
	}

	// Player Character
	{
		m_pPlayerCharacter = std::make_shared<PlayerCharacter>();
		m_pPlayerCharacter->Init();
		m_jumpSpeed= j.contains("PlayerCharacter")?j["PlayerCharacter"].value("JumpSpeed", PlayerJumpSpeed) : PlayerJumpSpeed;
		m_pPlayerCharacter->SetJumpSpeed(m_jumpSpeed);
		float m_moveSpeed = j.contains("PlayerCharacter") ? j["PlayerCharacter"].value("MoveSpeed", PlayerMoveSpeed) : PlayerMoveSpeed;
		m_pPlayerCharacter->SetMoveSpeed(m_moveSpeed);
		m_negativeStatusScale = j.contains("PlayerCharacter") ? j["PlayerCharacter"].value("NegativeStatusScale", NegativeSpeedEffector) : NegativeSpeedEffector;
	}

	//PlayerController初期化
	m_pPlayerController = std::make_unique<PlayerController>(this, m_pPlayerCharacter.get());
	PlayerController* controller = m_pPlayerController.get();

	//CameraController初期化
	{
		m_pCameraController = std::make_shared<CameraController>();
		DirectX::XMFLOAT3 camPos = DefaultCameraPos;
		DirectX::XMFLOAT3 camTarget = DefaultCameraTarget;
		DirectX::XMFLOAT3 camOffset = PlayerEyeHeight; // Default camera offset

		if (j.contains("CameraController"))
		{
			auto& jc = j["CameraController"];
			if (jc.contains("CameraPos") && jc["CameraPos"].is_array() && jc["CameraPos"].size() == 3)
			{
				camPos.x = jc["CameraPos"][0].get<float>();
				camPos.y = jc["CameraPos"][1].get<float>();
				camPos.z = jc["CameraPos"][2].get<float>();
			}
			if (jc.contains("CameraTarget") && jc["CameraTarget"].is_array() && jc["CameraTarget"].size() == 3)
			{
				camTarget.x = jc["CameraTarget"][0].get<float>();
				camTarget.y = jc["CameraTarget"][1].get<float>();
				camTarget.z = jc["CameraTarget"][2].get<float>();
			}
			if (jc.contains("CameraOffset") && jc["CameraOffset"].is_array() && jc["CameraTarget"].size() == 3)
			{
				camOffset.x = jc["CameraOffset"][0].get<float>();
				camOffset.y = jc["CameraOffset"][1].get<float>();
				camOffset.z = jc["CameraOffset"][2].get<float>();
			}
		}

		m_pCameraController->GetCamera()->SetPos(camPos);
		m_pCameraController->GetCamera()->SetTarget(camTarget);
		m_pCameraController->SetCameraOffset(camOffset);
		
	}

	// hunger component初期化→初期値
	{
		float hungerInit = j.contains("HungerComponent")
			? j["HungerComponent"].value("InitialValue", HungerInitialValue)
			: HungerInitialValue;

		float hungerSpeed = j.contains("HungerComponent")
			? j["HungerComponent"].value("StarveSpeed", HungerSpeed)
			: HungerSpeed;

		float hungerThreshold =j.contains("HungerComponent")
			? j["HungerComponent"].value("StarveThreshHold", HungerThreshold)
			: HungerThreshold;

		m_pHungerComponent = std::make_shared<HungerComponent>(hungerInit);
		m_pHungerComponent->Init(hungerSpeed, hungerThreshold);
		AddComponent(MyComponent::ComponentType::Hunger, m_pHungerComponent);

		// Add callback event to listener hungry
		m_pHungerComponent->AddHungryListener([this](bool isHungry)
		{
			//Check status
			m_isInNegativeState = isHungry || m_pThirstComponent->GetIsThirsty();

			//Camera shake& move speed down
			OnNegativeStateChanged();
			
		}); 

		// Add callback event to listener starve
		m_pHungerComponent->AddStarveListener([this](bool isStarve)
		{
			// get tick damage per sec
			m_isInDamagedStatus = isStarve || m_pThirstComponent->GetIsThirstyToDeath();
			OnStarveStateChanged();
		});	

	}


	// thirst component初期化→初期値
	{
		float thirstInit = j.contains("ThirstComponent")
			? j["ThirstComponent"].value("InitialValue", ThirstInitialValue)
			: ThirstInitialValue;
		float thirstSpeed = j.contains("ThirstComponent")?
			j["ThirstComponent"].value("ThirstSpeed", ThirstSpeed)
			: ThirstSpeed;
		float hungerThreshold = j.contains("ThirstComponent")
		? j["ThirstComponent"].value("ThirstThreshHold", ThirstThreshold)
			: ThirstThreshold;
		m_pThirstComponent = std::make_shared<ThirstComponent>(thirstInit);
		m_pThirstComponent->Init(thirstSpeed, hungerThreshold);
		AddComponent(MyComponent::ComponentType::Thirst, m_pThirstComponent);

		// Add callback event to listener thirsty
		m_pThirstComponent->AddThirstyListener([this](bool isThirsty)
		{
			//Check status
			m_isInNegativeState = isThirsty || m_pHungerComponent->GetIsHungry();

			OnNegativeStateChanged();
		});

		// Add Callback event to listener damaged
		m_pThirstComponent->AddThirstyToDeathListener([this](bool isThirstToDeath)
		{
			m_isInDamagedStatus = isThirstToDeath || m_pHungerComponent->GetIsStarve();

			OnStarveStateChanged();
		});	// get tick damage per sec

	}

	// Inventory
	{
		int inventorySize = j.contains("Inventory")
			? j["Inventory"].value("Size", PlayerInventorySize)
			: PlayerInventorySize;
		m_pInventory = std::make_shared<Inventory>(inventorySize);
	}


	AddDeathListener([controller](bool isDead)
		{
			controller->SetActive(!isDead); //if player dies, can't control player move
		}
	);

	AddDeathListener([](bool isDead)
	{
			if (isDead)ShowCursor(TRUE);
			else ShowCursor(FALSE);
	});

	return true;
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

		ImGui::Text("MoveSpeed:%f", m_pPlayerCharacter->GetMoveSpeed());

	}

	ImGui::End();
#endif

	// Death check
	if (m_pPlayerCharacter->GetPosition().y < -1.f)
		Kill();


	//　物理挙動更新
	m_pPlayerCharacter->Update(dt);
	m_pPlayerCharacter->SyncPlayerWorldPosition(m_transform);	//Transformを更新

	//=======Camera Update
	m_pCameraController->UpdateCameraTransform(m_transform);		//playerのTransformをCameraControllerに反映
	m_pCameraController->Update(dt);

	//=======Input
	m_pPlayerController->SetControllable(m_pCameraController->GetFirstPersonCamera());	//カメラがFirstPersonCameraなら操作可能
	m_pPlayerController->Update(dt);

	//=======Status Update
	m_pHungerComponent->Update(dt);	//空腹度
	m_pThirstComponent->Update(dt);	//渇き度

	//=======Inventory Update
	m_pInventory->Update(dt);

	//=======PlayerEntity Update
	m_pPlayerEntity->Update(dt);	//PlayerのHPを更新
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
	if (rotation.x > RotateLimit)
		rotation.x = RotateLimit;
	else if (rotation.x < -RotateLimit)
		rotation.x = -RotateLimit;

	m_transform.SetRotation(rotation);
}

void Player::RotateY(float dt)
{
	float rad = m_moveSpeed * dt;
	DirectX::XMFLOAT3 rotation = m_transform.GetRotation();
	rotation.y = DirectX::XMScalarModAngle(rotation.y + rad);
	m_transform.SetRotation(rotation);
}


void Player::AddDeathListener(const PlayerEntity::Callback& cb)
{
	m_pPlayerEntity->AddDeathListener(cb);
}

void Player::Revive()
{
	m_pPlayerEntity->Revive();

	// Reset position to avoid falling through the ground
	DirectX::XMFLOAT3 pos = { 0,PlayerEyeHeight.y,0 };
	m_pPlayerCharacter->SetPosition(pos);

	// todo: Inventory should be 2/3 lost when player dies

}

void Player::OnNegativeStateChanged()
{
	// Set camera shake
	m_pCameraController->OnNegativeStateChanged(m_isInNegativeState);

	// Set Player move speed
	float moveEffector = m_isInNegativeState ?
		m_negativeStatusScale : 1.f;	// 空腹状態なら速度減衰係数を適用

	m_pPlayerCharacter->SetMoveSpeed(m_moveSpeed * moveEffector);	// PlayerCharacterに速度を設定
	m_pPlayerCharacter->SetJumpSpeed(m_jumpSpeed * moveEffector);	// PlayerCharacterにジャンプ速度を設定

}

void Player::OnStarveStateChanged()
{
	m_pPlayerEntity->OnStateStarveChanged(m_isInDamagedStatus);
}


void Player::InteractWithObject(BodyID& id)
{
	if (GetItemInHand())
	{
		GetItemInHand()->InteractWith(id, this);
	}

	PhysicsComponent* component = PhysicsManager::Instance().GetPhysicsComponent(id);
	if (!component)return;
	GameObject* object = component->GetGameObject();

	// 当たったのがアイテムなら、インヴェントリーに追加
	if (object->GetGameObjectType() == GameObject::GameObjectType::Item)
	{
		auto item = dynamic_cast<ItemInstance*>(object);

		//todo: turn into switch
		//======== if item type is food/base material pick up
		if(item->GetProto()->GetItemType()==Item::ItemType::Food)
		{
			std::string name = item->GetName();
			int count = item->GetCount();
			int insertNum = GetInventory()->Insert(item);
			// アイテム全部挿入したら、しーんから消す
			if (count == insertNum)
			{
				item->SetState(ItemState::WaitToRecycle);    // Mark for recycling in DriftManager
				item->DeActivate();
			}
		}else if(item->GetProto()->GetItemType() == Item::ItemType::BaseMaterial)
		{
			std::string name = item->GetName();
			int count = item->GetCount();
			int insertNum = GetInventory()->Insert(item);
			// アイテム全部挿入したら、しーんから消す
			if (count == insertNum)
			{
				item->SetState(ItemState::WaitToRecycle);    // Mark for recycling in DriftManager
				item->DeActivate();
			}
		}

		

		//========= if item type is building
		// Todo: interact with building



	}
	else// If the hit object is not an item
	{
#ifdef _DEBUG
		
		DebugLog::Log("Hit object is not an item.");
#endif
	}

}

void Player::PickUpItem(BodyID& id)
{
	PhysicsComponent* component = PhysicsManager::Instance().GetPhysicsComponent(id);
	if (!component)return;
	GameObject* object = component->GetGameObject();

	// 当たったのがアイテムなら、インヴェントリーに追加
	if (object->GetGameObjectType() == GameObject::GameObjectType::Item)
	{
		auto item = dynamic_cast<ItemInstance*>(object);

		//todo: turn into switch
		//======== if item type is food/base material pick up
		if (item->GetProto()->GetItemType() == Item::ItemType::Food||
			item->GetProto()->GetItemType() == Item::ItemType::BaseMaterial)
		{
			std::string name = item->GetName();
			int count = item->GetCount();
			int insertNum = GetInventory()->Insert(item);
			// アイテム全部挿入したら、しーんから消す
			if (count == insertNum)
			{
				item->SetState(ItemState::WaitToRecycle);    // Mark for recycling in DriftManager
				item->DeActivate();
			}
#ifdef _DEBUG
			DebugLog::Log("Insert {} {}", insertNum, name);
#endif
		}
	}
}
