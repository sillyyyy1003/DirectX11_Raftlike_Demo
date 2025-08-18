#pragma once
#include "UIBar.h"
#include "UIComponent.h"

///@brief プレイヤーのステータスを表示するUIコンポーネント
class UIPlayerStatus :
	public UIComponent
{
public:
	typedef std::vector<Material*> MaterialList;
	enum MaterialType : uint8_t
	{
		Icon = 0,
		Background = 1,
		Bar = 2
	};

	UIPlayerStatus();
	~UIPlayerStatus() override = default;

	/// @brief 初期化
	void Init(Material* iconMaterial, Material* barBgMaterial, Material* barMaterial, IEffect* effect, Primitive* model);

	void Init(Material* iconMaterial, Material* barBgMaterial, Material* barMaterial, IEffect* effect, Primitive* model, Texture* hpIconTex, Texture* hungerIconTex);

	void Init(const MaterialList& hpMaterials, const MaterialList& hungerMaterials, IEffect* effect, Primitive* model);


	/// @brief 更新処理
	void Update(float dt) override;
	/// @brief 描画処理
	void Draw() override;
	/// @brief ステータスバーの位置を設定
	void SetPosition(const DirectX::XMFLOAT3& pos) {}
	void SetSize(const DirectX::XMFLOAT3& iconSize, const DirectX::XMFLOAT3& barSize);

	void InitPositionAndSize(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& iconSize, const DirectX::XMFLOAT3& barSize, float distanceBetweenBars);

	void LoadPositionAndSize(const char* fileName);
	void SetPlayer(Player* player) { m_pPlayer = player; }

private:
	std::unique_ptr<UIBar> m_pHealthBar;		// 体力バー
	std::unique_ptr<UIBar> m_pHungerBar;		// 飢餓バー
	//std::unique_ptr<UIBar> m_pThirstBar;      // 渇きバー

	std::unique_ptr<UIRender> m_pHpIcon;
	std::unique_ptr<UIRender> m_pHungerIcon;

	Player* m_pPlayer;

};

