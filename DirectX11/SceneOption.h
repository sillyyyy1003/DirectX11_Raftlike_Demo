#pragma once
#include "SceneBase.hpp"

class SceneOption:
	public SceneBase
{
public:
	SceneOption();
	~SceneOption() override = default;
	void Init() override;
	void UnInit() override;
	void Update(float tick) override;
	void Draw() override;

};

