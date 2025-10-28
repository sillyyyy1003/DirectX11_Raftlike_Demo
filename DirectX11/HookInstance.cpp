#include "HookInstance.h"

#include "d3dUtil.h"
#include "Player.h"

namespace
{
	static constexpr DirectX::XMFLOAT3 ItemDefaultOffset = { 0.3f,-0.3f,1.2f };
	enum ObjectState :int
	{
		Active = 0,         // Spawned and waiting to be picked up
		InActive = 1,       // Can be spawned
		WaitToRecycle = 2,  // Marked for recycling
		Hooked = 3          // hooked by player
	};
}

void HookInstance::OnUseStart(Player* player)
{
	if(m_hookState==Idle)
	{
		m_hookState = Charging;
		m_chargeTime = 0;
		// start to play se
	}if (m_hookState == Caught)
	{
		m_velocity = {};	// reset velocity
		m_hookState = Retrieving;
	}
}

void HookInstance::OnUseHold(Player* player, float deltaTime)
{
	const Hook* protoPtr = dynamic_cast<const Hook*>(GetProto().get());
	if(m_hookState==Charging)
	{
		float chargeSpeed = protoPtr->GetChargeSpeed();
		float timeLimit = protoPtr->GetChargeLimit();

		if (m_hookState != Charging)return;
		m_chargeTime += deltaTime * chargeSpeed;

		// clamp charge time to 0~timeLimit
		m_chargeTime = std::min(m_chargeTime, timeLimit);


	}else if(m_hookState== Retrieving)
	{
		// Set hook velocity  only in xz plane
		RVec3 dirToPlayer = {
			player->GetTransform().GetPosition().x - m_transform.GetPosition().x,
			0.f,
			player->GetTransform().GetPosition().z - m_transform.GetPosition().z
		};
		m_velocity = {
			dirToPlayer.Normalized().GetX(),
			0.f ,
			dirToPlayer.Normalized().GetZ()
		};

		// Reset hook
		if (dirToPlayer.Length() < 1.f)
		{
			m_chargeTime = 0;
			m_throwSpeed = 0;
			m_velocity = {};

			// set item states
			for(auto& item:m_caughtItems)
			{
				item->SetState(WaitToRecycle);
				player->GetInventory()->Insert(item);
			}

			m_caughtItems.clear();	// clear all game objects;

			m_transform.SetParent(&(player->GetCameraController()->GetCamera()->m_transform), ItemDefaultOffset);

			m_hookState = Idle;
			m_durability--;

			if(m_durability<=0)
			{
				// remove hook from inventory
				player->GetInventory()->RemoveCurrentSlotItem(1);
				player->GetInventory()->UpdateItemOfPlayer(player);
			}
		}
	}

}


void HookInstance::OnUseRelease(Player* player)
{
	if (m_hookState == Charging)
	{
		m_hookState = Casting;

		// Get throw velocity
		const Hook* protoPtr = dynamic_cast<const Hook*>(GetProto().get());
		float minSpeed = protoPtr->GetMinSpeed();
		float maxSpeed = protoPtr->GetMaxSpeed();
		float maxChargeTime = protoPtr->GetChargeLimit();

		// Lerp between min and max speed based on charge time
		m_throwSpeed = Lerp(minSpeed, maxSpeed, m_chargeTime / maxChargeTime);

		// Set Start Pos
		m_transform.SetPosition(m_transform.GetWorldPosition());

		m_velocity = m_transform.GetParent()->GetForwardAxis();
		// Set transform parent nullptr
		m_transform.SetParent(nullptr);
			
	}
	else if (m_hookState == Retrieving)
	{
		m_velocity = {};
	}

	//end se
}


void HookInstance::Update(float dt)
{
#ifdef _DEBUG
	if (ImGui::Begin("HookState"))
	{
		// Show charge time
		ImGui::Text("Charge Time: %f", m_chargeTime);

		// Show hook state
		ImGui::Text("Hook State: %d", m_hookState);

		// Show throw speed
		ImGui::Text("Throw Speed: %f", m_throwSpeed);

		ImGui::Text("Pos:%f,%f,%f", m_transform.GetPosition().x, m_transform.GetPosition().y, m_transform.GetPosition().z);

		ImGui::Text("Velocity:%f, %f, %f", m_velocity.x, m_velocity.y, m_velocity.z);
	}

	ImGui::End();
#endif

	switch (m_hookState)
	{
	case HookState::Idle:
		break;
	case HookState::Charging:
		break;
	case HookState::Casting:
		if (m_transform.GetPosition().y >= 0.f)
		{
			float gravityEffector = 0.1f, gravity = 9.81f;
			m_velocity.y -= gravity * gravityEffector * dt;	// gravity effect
			m_transform.Translate(m_velocity, m_throwSpeed * dt);
		}else
		{
			m_hookState=Caught;
		}
		break;
	case HookState::Caught:

		break;
	case HookState::Retrieving:
		m_transform.Translate(m_velocity, m_throwSpeed * dt);
		for(auto& item : m_caughtItems)
		{
			// Move caught items to hook position
			item->GetTransform().SetPosition(m_transform.GetPosition());
		}


		break;
	}

	GameObject::Update(dt);
}

float HookInstance::GetChargeProgress()
{
	if(m_hookState==Charging)
	{
		const Hook* protoPtr = dynamic_cast<const Hook*>(GetProto().get());
		float timeLimit = protoPtr->GetChargeLimit();
		return m_chargeTime /  timeLimit;
	}

	return 0.f;
}

