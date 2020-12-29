#include "cspch.h"
#include "InputComponent.h"
#include "Crystal/Core/ApplicationUtility.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"

namespace Crystal {

	void InputComponent::BindAxis(const std::string& axisName, const std::function<void(float value)>& function)
	{
		m_AxisFunctionmap.insert(std::make_pair(axisName, function));
	}

	void InputComponent::BindAction(const std::string& actionName, EKeyStatus keyEventType, const std::function<void(void)>& function)
	{
		m_ActionFunctionMap.insert(std::make_pair(std::make_pair(actionName, keyEventType), function));
	}

	bool InputComponent::ProcessInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		auto axisMap = ApplicationUtility::GetPlayerController()->GetAxisMap();
		auto actionMap = ApplicationUtility::GetPlayerController()->GetActionMap();

		bool bHandledOnAxis = false;
		bool bHandledOnAction = false;

		/*Process Axis*/
		auto axisIt = axisMap.find(wParam);
		if (axisIt != axisMap.end())
		{
			std::string axisName = (*axisIt).second.first;
			float axisScale = (*axisIt).second.second;
			auto axisFnIt = m_AxisFunctionmap.find(axisName);
			if (axisFnIt != m_AxisFunctionmap.end())
			{
				bHandledOnAxis = true;
				auto axisFn = (*axisFnIt).second;
				/*Call Axis Function*/
				axisFn(axisScale);
			}
		}

		ActionKey actionKey = {};
		actionKey.KeyCode = wParam;
		actionKey.bAltDown = GetKeyState(VK_MENU) & 0x8000;
		actionKey.bCtrlDown = GetKeyState(VK_CONTROL) & 0x8000;
		actionKey.bShiftDown = GetKeyState(VK_SHIFT) & 0x8000;

		EKeyStatus keyStatus = (HIWORD(lParam) & KF_REPEAT) ? EKeyStatus::KS_Repeat : EKeyStatus::KS_Pressed;
		keyStatus = uMsg == WM_KEYUP ? EKeyStatus::KS_Released : keyStatus;

		/*Process Action*/
		auto actionIt = actionMap.find(actionKey);
		if (actionIt != actionMap.end())
		{
			std::string actionName = (*actionIt).second;
			auto actionFnIt = m_ActionFunctionMap.find(std::make_pair(actionName, keyStatus));
			if (actionFnIt != m_ActionFunctionMap.end())
			{
				bHandledOnAction = true;
				auto actionFn = (*actionFnIt).second;
				/*Call Action Function*/
				actionFn();
			}
		}


		return bHandledOnAction || bHandledOnAxis;	
	}

}