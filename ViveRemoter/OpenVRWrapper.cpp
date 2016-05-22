#include "stdafx.h"
#include "OpenVRWrapper.h"


OpenVRWrapper::OpenVRWrapper()
	: m_spController1(nullptr),
	  m_spController2(nullptr)
{
}


OpenVRWrapper::~OpenVRWrapper()
{
}

void OpenVRWrapper::OutputDebugMsg(const WCHAR* szFormat, ...)
{
	WCHAR szBuff[1024];
	va_list arg;
	va_start(arg, szFormat);
	_vsnwprintf_s(szBuff, sizeof(szBuff), szFormat, arg);
	va_end(arg);

	OutputDebugString(szBuff);
}

bool OpenVRWrapper::Init()
{
	vr::EVRInitError eError = vr::VRInitError_None;
	//m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Background);
	m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Overlay);

	if (eError != vr::VRInitError_None)
	{
		m_pHMD = NULL;
		OutputDebugMsg(_T("Unable to init VR runtime: %s"), vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		return false;
	}
	for (int i = vr::k_unTrackedDeviceIndex_Hmd + 1; i < vr::k_unMaxTrackedDeviceCount; ++i)
	{
		if( m_pHMD->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_Controller)
		{
			if (m_spController1 == nullptr)
			{
				m_spController1.reset(new Controller(i));
			}
			else
			{
				m_spController2.reset(new Controller(i));
			}
		}
	}
	return true;
}

void OpenVRWrapper::Shutdown()
{
	if (m_pHMD)
	{
		vr::VR_Shutdown();
		m_pHMD = nullptr;
	}
}


void OpenVRWrapper::ProcessVREvent()
{
	vr::VREvent_t vEvent;
	bool bSuccess = m_pHMD->PollNextEvent(&vEvent, sizeof(vEvent));
	if (bSuccess) {
		vr::TrackedDeviceClass deviceType = m_pHMD->GetTrackedDeviceClass(vEvent.trackedDeviceIndex);
		switch (deviceType) {
		case vr::TrackedDeviceClass_HMD:
			ProcessHMDEvent(vEvent);
			break;
		case vr::TrackedDeviceClass_Controller:
			ProcessControllerEvent(vEvent);
			break;
		case vr::TrackedDeviceClass_TrackingReference:
			break;
		default:
			OutputDebugMsg(_T("VREVENT: %d %d\n"), deviceType, vEvent.eventType);
			break;
		}
	}
}

void OpenVRWrapper::ProcessHMDEvent(const vr::VREvent_t & event)
{
	switch (event.eventType)
	{
	case vr::VREvent_TrackedDeviceRoleChanged:
    //TODO: Switch controller 1,2
    //TODO: check index really change?
		OutputDebugMsg(_T("VREvent_TrackedDeviceRoleChanged\n"));
		break;
	default:
		OutputDebugMsg(_T("VREvent HMD %d\n"), event.eventType);
		break;
	}
}
void OpenVRWrapper::ProcessControllerEvent(const vr::VREvent_t & event)
{
	std::shared_ptr<Controller> spController;
	if(m_spController1 == nullptr || m_spController1->Index == event.trackedDeviceIndex )
	{
		m_ControllerEvent.Index = 0;
		spController = m_spController1;
	}
	else
	{
		m_ControllerEvent.Index = 1;
		spController = m_spController2;
	}

	if (spController == nullptr)
	{
		spController.reset(new Controller(event.trackedDeviceIndex));
	}

	switch (event.eventType)
	{
	case vr::VREvent_TrackedDeviceActivated:
	{
		if (m_spController1 == nullptr || m_spController1->Index == event.trackedDeviceIndex)
		{
			m_spController1 = spController;
		}
		else
		{
			m_spController2 = spController;
		}
		OutputDebugMsg(_T("VREvent_TrackedDeviceActivated\n"));
	}
	break;
	case vr::VREvent_TrackedDeviceDeactivated:
	{
		//TODO: Remove Controller
		//TODO: Check what happened to the index of controller 2, if controller 1 removed
		
		if (m_spController1 == spController) {
			m_spController1.reset();
		}
		else {
			m_spController2.reset();
		}
		spController.reset();
		OutputDebugMsg(_T("VREvent_TrackedDeviceDeactivated\n"));
	}
	case vr::VREvent_ButtonPress:
	{
		spController->SetPress(event.data.controller.button, true);
		vr::VRControllerState_t controllerState;
		if (m_pHMD->GetControllerState(event.trackedDeviceIndex, &controllerState))
		{
			spController->SetPos(controllerState.rAxis[0].x, controllerState.rAxis[0].y);
		}		
		OutputDebugMsg(_T("VREvent_ButtonPress\n"));
	}
	break;
	case vr::VREvent_ButtonUnpress:
	{
		spController->SetPress(event.data.controller.button, false);
		vr::VRControllerState_t controllerState;
		if (m_pHMD->GetControllerState(event.trackedDeviceIndex, &controllerState))
		{
			spController->SetPos(controllerState.rAxis[0].x, controllerState.rAxis[0].y);
		}
		OutputDebugMsg(_T("VREvent_ButtonUnpress\n"));
	}
	break;
	case vr::VREvent_ButtonTouch:
	{
		spController->SetTouch(event.data.controller.button, true);
		vr::VRControllerState_t controllerState;
		if (m_pHMD->GetControllerState(event.trackedDeviceIndex, &controllerState))
		{
			spController->SetPos(controllerState.rAxis[0].x, controllerState.rAxis[0].y);
		}
		OutputDebugMsg(_T("VREvent_ButtonTouch\n"));
	}
	break;
	case vr::VREvent_ButtonUntouch:
	{
		spController->SetTouch(event.data.controller.button, false);
		vr::VRControllerState_t controllerState;
		if (m_pHMD->GetControllerState(event.trackedDeviceIndex, &controllerState))
		{
			spController->SetPos(controllerState.rAxis[0].x, controllerState.rAxis[0].y);
		}
		OutputDebugMsg(_T("VREvent_ButtonUntouch\n"));
	}
	break;
	case vr::VREvent_TouchPadMove:
	{
		spController->SetPos(event.data.mouse.x, event.data.mouse.y);
		OutputDebugMsg(_T("VREvent_TouchPadMove %d %d\n"), event.data.mouse.x, event.data.mouse.y);
	}
	default:
		OutputDebugMsg(_T("VREvent Controller %d\n"), event.eventType);
		break;
	}

	m_ControllerEvent.Button = event.data.controller.button;
	m_ControllerEvent.EventType = event.eventType;
	m_ControllerEvent.X = spController->TPadX;
	m_ControllerEvent.Y = spController->TPadY;
}

void OpenVRWrapper::UpdateController()
{
	if (m_spController1 == nullptr)
		return;

	if (m_spController1->TPadTouch)
	{
		vr::VRControllerState_t controllerState;
		if (m_pHMD->GetControllerState(m_spController1->Index, &controllerState))
		{
			m_spController1->SetPos(controllerState.rAxis[0].x, controllerState.rAxis[0].y);
		}
	}

	//if (m_spController2->TPadTouch)
	//{
	//	vr::VRControllerState_t controllerState;
	//	if (m_pHMD->GetControllerState(m_spController1->Index, &controllerState))
	//	{
	//		m_spController1->SetPos(controllerState.rAxis[0].x, controllerState.rAxis[0].y);
	//	}
	//}
}

Controller::Controller(int index)
	: Index(index),
	TriggerPress(false),
	TriggerTouch(false),
	TPadPress(false),
	TPadTouch(false),
	TPadX(0),
	TPadY(0),
	GripPress(false),
	MenuPress(false)
{
}


void Controller::SetPress(uint32_t button, bool bPress)
{
	switch (button) {
	case vr::EVRButtonId::k_EButton_SteamVR_Trigger:
		TriggerPress = bPress;
		break;
	case vr::EVRButtonId::k_EButton_SteamVR_Touchpad:
		TPadPress = bPress;
		break;
	case vr::EVRButtonId::k_EButton_ApplicationMenu:
		MenuPress = bPress;
		break;
	case vr::EVRButtonId::k_EButton_Grip:
		GripPress = bPress;
		break;
	default:
		break;
	}
}
void Controller::SetTouch(uint32_t button, bool bTouch)
{
	switch (button) {
	case vr::EVRButtonId::k_EButton_SteamVR_Trigger:
		TriggerTouch = bTouch;
		break;
	case vr::EVRButtonId::k_EButton_SteamVR_Touchpad:
		TPadTouch = bTouch;
		break;
	default:
		break;
	}
}
void Controller::SetPos(float x, float y)
{
	TPadX = x;
	TPadY = y;
}

void ControllerEvent::Reset()
{
	Index = -1;
	Button = -1;
	EventType = -1;
	X = 0;
	Y = 0;
}