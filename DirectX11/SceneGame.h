#pragma once
#include <vector>

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

	void RegisterSceneObject(std::shared_ptr<GameObject> object);

	void DeleteInactiveSceneObject();

private:
	CameraBase* m_pCurrentCamera;

	typedef std::vector<std::shared_ptr<GameObject>> Objects;
	Objects m_sceneObjects;	// 活動しているGameObjects
};

