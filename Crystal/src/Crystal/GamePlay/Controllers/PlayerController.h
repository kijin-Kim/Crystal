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
		IM_Game, // ���� ��
		IM_UI, // UI ��
	};

	class PlayerController : public Controller
	{
	public:
		PlayerController();
		virtual ~PlayerController() = default;

		void AddAxisMapping(const std::string& axisName, int key, float scale);
		void AddActionMapping(const std::string& actionName, const ActionMapping& key);

		/*������Ʈ�ѷ��� Ư�� ���� �Է��� �ް��մϴ�*/
		void Possess(Pawn* pawn) override;
		bool OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

		void ProcessPitchInput(float value);
		void ProcessYawInput(float value);

		void SetInputMode(EInputMode inputMode) { m_InputMode = inputMode; }
		/*UI���� Game��带 Ư�� Ű�� ���Ͽ� ����ġ �� �� �ְ� �մϴ�. ����Ʈ ������ ���콺 Ű*/
		void EnableModeSwitching(bool bEnable, int64_t keyCode = Crystal::Mouse::Right);

		void SetMainCamera(CameraComponent* cameraComponent) { m_MainCamera = cameraComponent; }
		CameraComponent* GetMainCamera() const { return m_MainCamera; }

		const std::map<int64_t, std::pair<std::string, float>>& GetAxisMap() const { return m_AxisMap; }
		const std::map<ActionMapping, std::string, ActionKeyCompare>& GetActionMap() const { return m_ActionMap; }

		STATIC_TYPE_IMPLE(PlayerController)
	private:

		/*���� �������̽� ��ǲ*/
		std::unique_ptr<InputComponent> m_UserInterfaceInputComponent = nullptr;
		/*���Ӹ�� ��ǲ*/
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
