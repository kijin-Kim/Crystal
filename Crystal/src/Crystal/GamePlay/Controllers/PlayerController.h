#pragma once
#include <map>
#include "Controller.h"
#include "Crystal/GamePlay/Components/CameraComponent.h"

namespace Crystal {

	class PlayerController : public Controller
	{
	public:
		PlayerController() = default;
		virtual ~PlayerController()
		{
			for (InputComponent* inputComponent : m_InputComponents)
				delete inputComponent;
		}

		void AddAxisMapping(const std::string& axisName, int key, int scale)
		{
			m_AxisMap.insert(std::make_pair(key, std::make_pair(axisName, scale)));
		}

		void AddActionMapping(const std::string& actionName, int key)
		{
			m_ActionMap.insert(std::make_pair(key, actionName));
		}
		
		virtual void Possess(Pawn* pawn) override
		{
			Controller::Possess(pawn);
			m_InputComponents.push_back(new InputComponent());
			pawn->SetupInputComponent(m_InputComponents.back());
		}

		virtual void OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override
		{
			for (InputComponent* inputComponent : m_InputComponents)
			{
				bool bHandled = inputComponent->ProcessInputEvent(hWnd, uMsg, wParam, lParam);
				if (bHandled)
					break;
			}
					
		}

		void SetMainCamera(CameraComponent* cameraComponent) { m_MainCamera = cameraComponent; }
		CameraComponent* GetMainCamera() const { return m_MainCamera; }

		const std::map<int, std::pair<std::string, int>>& GetAxisMap() const { return m_AxisMap; }
		const std::map<int, std::string>& GetActionMap() const { return m_ActionMap; }

	private:
		std::vector<InputComponent*> m_InputComponents;
		/* KeyCode, AxisName, Scale */
		std::map<int, std::pair<std::string, int>> m_AxisMap;
		/* KeyCode, ActionName */
		std::map<int, std::string> m_ActionMap;

		CameraComponent* m_MainCamera = nullptr;
	};
}
