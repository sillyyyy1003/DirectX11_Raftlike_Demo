#include "UIBar.h"

#include "ModelManager.h"

UIBar::UIBar() :
	m_backgroundMesh(nullptr),
	m_barMesh(nullptr),
	m_scale(0,0,1),
	m_position(0,0,0)
{
}

void UIBar::Init(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT2& scale, Material* bgMaterial,
	Material* barMaterial, IEffect* bgEffect, IEffect* barEffect)
{
	m_barMesh = std::make_unique<UIMesh>();
	m_backgroundMesh = std::make_unique<UIMesh>();

	//Set pos & scale 
	m_position = pos;
	m_scale = { scale.x,scale.y,1.0f };

	m_barMesh->SetModel(ModelManager::Instance().GetModel("Square"));
	m_barMesh->SetMaterial(barMaterial);
	m_barMesh->SetEffect(barEffect);

	m_backgroundMesh->SetModel(ModelManager::Instance().GetModel("Square"));
	m_backgroundMesh->SetMaterial(bgMaterial);
	m_backgroundMesh->SetEffect(bgEffect);


	// Set Scale
	m_barMesh->GetTransform().SetScale(m_scale);
	// object実際の位置を計算（anchor Pos + scale/2）
	DirectX::XMFLOAT3 currPos = { m_position.x + m_scale.x / 2.f,m_position.y,m_position.z };
	m_barMesh->GetTransform().SetPosition(currPos);



	// 背景ちょっと大きい
	DirectX::XMFLOAT3 bgScale = { m_scale.x + 10.f,m_scale.y + 10.f ,m_scale.z };
	m_backgroundMesh->GetTransform().SetScale(bgScale);

	// 位置を真ん中+ Layerずらす
	currPos.z += 0.1f;
	m_backgroundMesh->GetTransform().SetPosition(currPos);

}

void UIBar::UpdateUI(float volume)
{
	//移動量・現在の長さを計算する
	float width = m_scale.x * volume;
	float desPos = m_position.x + width / 2.f;

	//Reset scale & pos
	DirectX::XMFLOAT3 currScale = m_barMesh->GetTransform().GetScale();
	DirectX::XMFLOAT3 currPos = m_barMesh->GetTransform().GetPosition();
	currScale.x = width;
	currPos.x = desPos;
	m_barMesh->GetTransform().SetScale(currScale);
	m_barMesh->GetTransform().SetPosition(currPos);
}

void UIBar::Draw(const char* string)
{
	m_barMesh->Draw();
	m_backgroundMesh->Draw();
	
}

void UIBar::Update(float dt)
{
}


