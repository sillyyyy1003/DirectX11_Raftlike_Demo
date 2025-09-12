#include "SceneOption.h"

#include "KInput.h"

SceneOption::SceneOption()
{
}

void SceneOption::Init()
{
}

void SceneOption::UnInit()
{
}

void SceneOption::Update(float tick)
{
	if(KInput::IsKeyTrigger(VK_ESCAPE))
	{
		SetCurrentScene("Title");
		return;
	}
}

void SceneOption::Draw()
{
}
