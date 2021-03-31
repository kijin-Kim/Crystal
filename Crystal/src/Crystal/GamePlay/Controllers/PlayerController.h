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

	enum class EInputMode
	{
		IM_Game, // 게임 만
		IM_UI, // UI 만
	};

	class PlayerController : public Controller
	{
	public:
		PlayerController();
		virtual ~PlayerController() = default;

		void AddAxisMapping(const std::string& axisName, int key, float scale);
		void AddActionMapping(const std::string& actionName, const ActionMapping& key);

		/*게임컨트롤러가 특정 폰의 입력을 받게합니다*/
		void Possess(Pawn* pawn) override;
		bool OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

		void ProcessPitchInput(float value);
		void ProcessYawInput(float value);

		void SetInputMode(EInputMode inputMode) { m_InputMode = inputMode; }
		/*UI모드와 Game모드를 특정 키를 통하여 스위치 할 수 있게 합니다. 디폴트 오른쪽 마우스 키*/
		void EnableModeSwitching(bool bEnable, int64_t keyCode = Crystal::Mouse::Right);

		void SetMainCamera(CameraComponent* cameraComponent) { m_MainCamera = cameraComponent; }
		CameraComponent* GetMainCamera() const { return m_MainCamera; }

		const std::map<int64_t, std::pair<std::string, float>>& GetAxisMap() const { return m_AxisMap; }
		const std::map<ActionMapping, std::string, ActionKeyCompare>& GetActionMap() const { return m_ActionMap; }

		STATIC_TYPE_IMPLE(PlayerController)
	private:

		/*유저 인터페이스 인풋*/
		std::unique_ptr<InputComponent> m_UserInterfaceInputComponent = nullptr;
		/*게임모드 인풋*/
		std::unique_ptr<InputComponent> m_GameInputComponent;
		/* KeyCode, AxisName, Scale */
		std::map<int64_t, std::pair<std::string, float>> m_AxisMap;
		/* KeyCode, ActionName */
		std::map<ActionMapping, std::string, ActionKeyCompare> m_ActionMap;
		CameraComponent* m_MainCamera = nullptr;

		EInputMode m_InputMode = EInputMode::IM_UI;
		bool m_bIsSwitchableMode = false;
	};
}
