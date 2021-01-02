#include "cspch.h"
#include "InputComponent.h"
#include "Crystal/Core/ApplicationUtility.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"

namespace Crystal {
	void InputComponent::BindAxis(const std::string& axisName, const std::function<void(float value)>& function)
	{
		m_AxisFunctionmap.insert(std::make_pair(axisName, function));
	}

	void InputComponent::BindAction(const std::string& actionName, EKeyEvent keyEventType, const std::function<void(void)>& function)
	{
		m_ActionFunctionMap.insert(std::make_pair(std::make_pair(actionName, keyEventType), function));
	}

	bool InputComponent::ProcessInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		
		auto [keyCode, keyStatus] = GetCrystalKeyCode(uMsg, wParam, lParam);
		ProcessAxisMappedInput(keyCode, 1.0f);

		ProcessActionMappedInput(uMsg, keyCode, lParam, keyStatus);

		MouseCodeWithDelta mouseCodes = GetCrystalMouseCodeWithDelta(uMsg, lParam);
		ProcessAxisMappedInput(mouseCodes.MouseX.first, mouseCodes.MouseX.second);
		ProcessAxisMappedInput(mouseCodes.MouseY.first, mouseCodes.MouseY.second);
		ProcessAxisMappedInput(mouseCodes.VWheel.first, mouseCodes.VWheel.second);
		ProcessAxisMappedInput(mouseCodes.HWheel.first, mouseCodes.HWheel.second);
		ProcessActionMappedInput(uMsg, mouseCodes.MouseX.first, lParam, EKeyEvent::KE_Pressed);
		ProcessActionMappedInput(uMsg, mouseCodes.MouseY.second, lParam, EKeyEvent::KE_Pressed);
		ProcessActionMappedInput(uMsg, mouseCodes.VWheel.first, lParam, EKeyEvent::KE_Pressed);
		ProcessActionMappedInput(uMsg, mouseCodes.HWheel.second, lParam, EKeyEvent::KE_Pressed);


		static UCHAR pKeysBuffer[256];
		for (int i = 0; i < 256; i++)
		{
			if (GetAsyncKeyState(i) & 0x8000)
			{
				auto [keyCode, keyStatus] = GetCrystalKeyCode(uMsg, i, lParam);
				ProcessAxisMappedInput(keyCode, 1.0f);
			}
		}		
		return false;
	}

	bool InputComponent::ProcessAxisMappedInput(int64_t keyCode, float axisValue)
	{
		auto axisMap = ApplicationUtility::GetPlayerController().GetAxisMap();
		bool bHandledOnAxis = false;

		/*Process Axis*/
		auto axisIt = axisMap.find(keyCode);
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
				axisFn(axisValue * axisScale);
			}
		}
		return bHandledOnAxis;

	}

	bool InputComponent::ProcessActionMappedInput(UINT uMsg, int64_t keyCode, LPARAM lParam, EKeyEvent keyStatus)
	{
		auto actionMap = ApplicationUtility::GetPlayerController().GetActionMap();

		bool bHandledOnAction = false;

		ActionMapping actionKey = {};
		actionKey.CrystalCode = keyCode;
		actionKey.bAltDown = GetKeyState(VK_MENU) & 0x8000;
		actionKey.bCtrlDown = GetKeyState(VK_CONTROL) & 0x8000;
		actionKey.bShiftDown = GetKeyState(VK_SHIFT) & 0x8000;


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

		return bHandledOnAction;
	}

}