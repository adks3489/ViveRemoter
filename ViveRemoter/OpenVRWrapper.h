#pragma once

#include "openvr.h"
#include <memory>

class ControllerEvent {
public:
	int Index;
	uint32_t EventType;
	uint32_t Button;
	float X;
	float Y;
	void Reset();
};

class Controller {
public:
	Controller(int index);
	int Index;
	bool TriggerPress;
	bool TriggerTouch;
	bool TPadPress;
	bool TPadTouch;
	float TPadX;
	float TPadY;
	bool GripPress;
	bool MenuPress;
	void SetPress(uint32_t button, bool bPress);
	void SetTouch(uint32_t button, bool bTouch);
	void SetPos(float x, float y);
};

class OpenVRWrapper
{
public:
	OpenVRWrapper();
	~OpenVRWrapper();
	void OutputDebugMsg(const WCHAR* szFormat, ...);

	bool Init();
	void Shutdown();

	void ProcessVREvent();
	void UpdateController();

	std::shared_ptr<Controller> m_spController1;
	std::shared_ptr<Controller> m_spController2;
	ControllerEvent m_ControllerEvent;
private:
	vr::IVRSystem* m_pHMD;
	void ProcessHMDEvent(const vr::VREvent_t & event);
	void ProcessControllerEvent(const vr::VREvent_t & event);
};
