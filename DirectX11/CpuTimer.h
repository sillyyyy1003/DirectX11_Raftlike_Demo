//***************************************************************************************
// CpuTimer.h by Frank Luna (C) 2011 All Rights Reserved.
// Modify name from GameTimer.cpp
// CPU Timer
//***************************************************************************************

#pragma once

class CpuTimer
{
public:
    CpuTimer();

    float TotalTime()const;     
    float DeltaTime()const;     

    void Reset();             
    void Start();              
    void Stop();                
    void Tick();               
    bool IsStopped() const;    

private:
    double m_SecondsPerCount = 0.0;
    double m_DeltaTime = -1.0;

    __int64 m_BaseTime = 0;
    __int64 m_PausedTime = 0;
    __int64 m_StopTime = 0;
    __int64 m_PrevTime = 0;
    __int64 m_CurrTime = 0;

    bool m_Stopped = false;
};

