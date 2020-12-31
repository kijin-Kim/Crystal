#pragma once
#include <map>
#include "Controller.h"
#include "Crystal/GamePlay/Components/CameraComponent.h"

namespace Crystal {
	struct ActionMapping
	{
		int64_t CrystalCode = 0;
		bool bAltDown = false;
		bool bCtrlDown = false;
		bool bShiftDown = false;
	};

	struct ActionKeyCompare
	{
		bool operator() (const ActionMapping& lhs, const ActionMapping& rhs) const
		{
			if (lhs.CrystalCode == rhs.CrystalCode)
			{
				if (lhs.bAltDown == rhs.bAltDown)
				{
					if (lhs.bCtrlDown == rhs.bCtrlDown)
						return lhs.bShiftDown < rhs.bShiftDown;
					return lhs.bCtrlDown < rhs.bCtrlDown;
				}
				return lhs.bAltDown < rhs.bAltDown;
			}
			return lhs.CrystalCode < rhs.CrystalCode;
		}
	};

	class PlayerController : public Controller
	{
	public:
		PlayerController() = default;
		virtual ~PlayerController()
		{
			for (InputComponent* inputComponent : m_InputComponents)
				delete inputComponent;
		}

		void AddAxisMapping(const std::string& axisName, int key, float scale)
		{
			auto result = m_AxisMap.insert(std::make_pair(key, std::make_pair(axisName, scale)));
			CS_ASSERT(result.second, "AxisMapping에 실패하였습니다.");
		}

		void AddActionMapping(const std::string& actionName, const ActionMapping& key)
		{
			auto result = m_ActionMap.insert(std::make_pair(key, actionName));
			CS_ASSERT(result.second, "ActionMapping에 실패하였습니다.");
		}

		virtual void Possess(Pawn* pawn) override
		{
			Controller::Possess(pawn);
			m_InputComponents.push_back(new InputComponent());
			pawn->SetupInputComponent(m_InputComponents.back());
		}

		virtual bool OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override
		{
			bool bHandled = false;
			for (InputComponent* inputComponent : m_InputComponents)
			{
				bHandled = inputComponent->ProcessInputEvent(hWnd, uMsg, wParam, lParam);
				if (bHandled)
					break;
			}
			return bHandled;
		}

		void SetMainCamera(CameraComponent* cameraComponent) { m_MainCamera = cameraComponent; }
		CameraComponent* GetMainCamera() const { return m_MainCamera; }

		const std::map<int64_t, std::pair<std::string, float>>& GetAxisMap() const { return m_AxisMap; }
		const std::map<ActionMapping, std::string, ActionKeyCompare>& GetActionMap() const { return m_ActionMap; }

	private:
		std::vector<InputComponent*> m_InputComponents;
		/* KeyCode, AxisName, Scale */
		std::map<int64_t, std::pair<std::string, float>> m_AxisMap;
		/* KeyCode, ActionName */
		std::map<ActionMapping, std::string, ActionKeyCompare> m_ActionMap;

		CameraComponent* m_MainCamera = nullptr;
	};
}
