#include "cspch.h"
#include "InputComponent.h"
#include "Crystal/Core/ApplicationUtility.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"

namespace Crystal {


	void InputComponent::BindAxis(const std::string& axisName, Pawn* owner, const std::function<void(Pawn*, float value)>& function)
	{
		m_AxisFunctionmap.insert(std::make_pair(axisName, std::bind(function, owner, std::placeholders::_1)));
	}

	void InputComponent::BindAction(const std::string& actionName, EKeyStatus keyEventType, Pawn* owner, const std::function<void(Pawn*)>& function)
	{
		m_ActionFunctionMap.insert(std::make_pair(std::make_pair(actionName, keyEventType), std::bind(function, owner)));
	}

	bool InputComponent::ProcessInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		auto axisMap = ApplicationUtility::GetPlayerController()->GetAxisMap();
		auto actionMap = ApplicationUtility::GetPlayerController()->GetActionMap();

		bool bHandledOnAxis = false;
		bool bHandledOnAction = false;

		/*Process Axis*/
		auto axisIt =axisMap.find(wParam);
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


		EKeyStatus keyStatus = EKeyStatus::KS_None;
		switch (uMsg)
		{
		case WM_KEYDOWN:
			if (!(HIWORD(lParam) & KF_REPEAT))
				keyStatus = EKeyStatus::KS_Pressed;
			else
				keyStatus = EKeyStatus::KS_Repeat;
			break;
		case WM_KEYUP:
			keyStatus = EKeyStatus::KS_Released;
			break;
		default:
			break;
		}


		/*Process Action*/
		auto actionIt =actionMap.find(wParam);
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