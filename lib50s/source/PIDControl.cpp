/*
	PIDControl.cpp
	Copyright (c) [ORGNAME] 2013
	Author: Timothy Yau
*/

#include <cmath>
#include <cfloat>

#include "..\\include\\Common.hpp"
#include "..\\include\\PIDControl.hpp"

using namespace std;

PIDControl::PIDControl(float kp, float ki, float kd, float setpoint, float maxOutput, float minOutput)
	: m_Kp(kp), m_Ki(ki), m_Kd(kd), m_Setpoint(setpoint), m_Integral(0), m_PrevVal(FLT_MAX),
	m_MaxOutput(maxOutput), m_MinOutput(minOutput)
{
}


PIDControl::~PIDControl(void)
{
}

float PIDControl::step(float feedbackVal) 
{
	float err = m_Setpoint - feedbackVal;

	//PRINT("PID err=" << err << ", integral=" << m_Integral << ", prevVal=" << m_PrevVal, WHITE);

	if (m_PrevVal == FLT_MAX)
	{
		m_PrevVal = feedbackVal;
	}

	float out = m_Kp * err + m_Ki * m_Integral + m_Kd * (m_PrevVal - feedbackVal);

	if (out > m_MaxOutput)
		out = m_MaxOutput; // Don't update integral to avoid windup
	else if (out < m_MinOutput)
		out = m_MinOutput;
	else
		m_Integral += err;

	m_PrevVal = feedbackVal;

	//PRINT(", Control=" << out << endl, WHITE);

	return out;
}

