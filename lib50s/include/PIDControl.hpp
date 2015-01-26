#ifndef __PIDCONTROL_HPP__
#define __PIDCONTROL_HPP__

#include <cfloat>

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Generic PID feedback controller implementation. 
 */

class PIDControl
{
public:
	PIDControl(float kp, float ki, float kd, float setpoint, 
		float maxOutput = FLT_MAX, float minOuput = -FLT_MAX);

	~PIDControl(void);

	// This function assumes a constant time step between each call.
	// feedbackVal is the current value of the process being controlled
	// return value is the control output, which should be positively 
	// correlated with the process value.
	float step(float feedbackVal);

	void setSetpoint(float newSetpoint)		{ m_Setpoint = newSetpoint; }
	float getSetpoint(void)					{ return m_Setpoint; }

	void setMaxOutput(float maxOutput)		{ m_MaxOutput = maxOutput; }
	float getMaxOutput(void)				{ return m_MaxOutput; }

	void setMinOutput(float minOutput)		{ m_MinOutput = minOutput; }
	float getMinOutput(void)				{ return m_MinOutput; }

protected:
	float m_Kp, m_Ki, m_Kd;
	float m_Setpoint;

	float m_MaxOutput, m_MinOutput;

	float m_Integral;
	float m_PrevVal;
};

#endif