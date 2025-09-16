#pragma once
#include <vector>

#include "SceneBase.hpp"


class CameraBase;
class SceneGame :
	public SceneBase
{

public:
	typedef std::vector<std::shared_ptr<GameObject>> Objects;
	SceneGame() = default;
	~SceneGame() override = default;

	void Init() override;
	void UnInit() override;
	void Update(float tick) override;
	void Draw() override;

	void RegisterSceneObject(std::shared_ptr<GameObject> object);

	void DeleteInactiveSceneObject();
	void SetCurrentScene(const char* sceneName) override;


private:
	CameraBase* m_pCurrentCamera;

	typedef std::vector<std::shared_ptr<GameObject>> Objects;
	Objects m_sceneObjects;	// 活動しているGameObjects

	float m_time = 0;

	bool m_isChangeScene = false;
	std::string m_sceneName;

	bool m_isShowCursor = false;		// if is pause show cursor	
};

