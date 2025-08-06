#pragma once
#include "DirLight.h"

class DayLight :
    public DirLight
{
public:
    void Update(float dt) override;

private:
    float m_tick = 0;
};

