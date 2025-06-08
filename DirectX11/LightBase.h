#pragma once
#include <DirectXCollision.h>

namespace Light
{

	/// @brief  方向光
	struct DirectionLight
	{
		DirectX::XMFLOAT4 ambient;	// 環境光 
		DirectX::XMFLOAT4 diffuse;	// 拡散反射
		DirectX::XMFLOAT3 direction;
		float isEnable;//起動するかどうか

	};

	/// @brief 点光源
	struct PointLight
	{
		DirectX::XMFLOAT4 ambient;	// 環境光 
		DirectX::XMFLOAT4 diffuse;	// 拡散反射
		DirectX::XMFLOAT3 position;
		float range = 1;

		DirectX::XMFLOAT3  attenuation;//減衰
		float isEnable = false;//起動するかどうか
	};

	/// @brief スポットライト
	struct Spotlight
	{

		DirectX::XMFLOAT4 ambient;	// 環境光 
		DirectX::XMFLOAT4 diffuse;	// 拡散反射 

		DirectX::XMFLOAT3 position;
		float range;

		DirectX::XMFLOAT3 direction; //向かうどころ
		float Spot;	//焦点?

		DirectX::XMFLOAT3  attenuation;//減衰
		float isEnable;//起動するかどうか
	};

}
class LightBase
{
protected:
	DirectX::XMFLOAT3 m_pos;
	DirectX::XMFLOAT3 m_dir;
	DirectX::XMFLOAT4 m_ambient;
	DirectX::XMFLOAT4 m_diffuse;

	bool isEnable = true;

public:
	LightBase();
	virtual  ~LightBase() = default;

	virtual void Update(float dt) = 0;
	DirectX::XMFLOAT3 GetPosition()const { return m_pos; };
	DirectX::XMFLOAT3 GetDirection()const { return m_dir; };
	DirectX::XMFLOAT4 GetAmbient()const { return m_ambient; };
	DirectX::XMFLOAT4 GetDiffuse()const { return m_diffuse; };

	bool GetEnable()const { return isEnable; };

	void SetPosition(const DirectX::XMFLOAT3& _pos);	//位置
	void SetPosition(float* _pos);

	void SetDirection(const DirectX::XMFLOAT3& _dir);	//方向
	void SetDirection(float* _dir);

	void SetAmbient(const DirectX::XMFLOAT4& _ambient); //環境光
	void SetAmbient(float* ambient);

	void SetDiffuse(const DirectX::XMFLOAT4& _diffuse); //拡散反射
	void SetDiffuse(float* diffuse);

	void SetEnable(bool isEnable) { this->isEnable = isEnable; };




};

