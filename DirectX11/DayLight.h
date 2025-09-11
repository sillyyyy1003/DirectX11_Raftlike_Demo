#pragma once
#include "DirLight.h"

class DayLight :
    public DirLight
{
public:
    DayLight();
    void Update(float dt) override;

private:
    float m_time = 0;
};

