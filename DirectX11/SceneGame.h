#pragma once
#include "SceneBase.hpp"


class CameraBase;
class SceneGame :
    public SceneBase
{
	
public:
	SceneGame() = default;
	~SceneGame() override = default;

	void Init() override;
	void UnInit() override;
	void Update(float tick) override;
	void Draw() override;


	
private:
	CameraBase* m_pCurrentCamera;
};

