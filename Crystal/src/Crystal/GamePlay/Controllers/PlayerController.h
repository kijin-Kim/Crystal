#pragma once
#include <map>
#include "Controller.h"
#include "Crystal/GamePlay/Components/CameraComponent.h"

namespace Crystal {
	struct ActionMapping
	{
		SERIALIZE_PROPERTIES
		{
			ar & CrystalCode;
			ar & bAltDown;
			ar & bCtrlDown;
			ar & bShiftDown;
		}

		int64_t CrystalCode = 0;
		bool bAltDown = false;
		bool bCtrlDown = false;
		bool bShiftDown = false;
	};

	struct ActionKeyCompare
	{
		bool operator()(const ActionMapping& lhs, const ActionMapping& rhs) const
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

	enum class EInputMode : uint32_t
	{
		IM_Game,
		// ���� ��
		IM_UI,
		// UI ��
	};

	class PlayerController : public Controller
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<Controller>(*this);
			ar & *m_UserInterfaceInputComponent;
			ar & *m_GameInputComponent;
			ar & m_AxisMap;
			ar & m_ActionMap;
			ar & m_InputMode;
			ar & m_bIsSwitchableMode;
			ar & m_NetworkId;
		}

	public:
		PlayerController();
		virtual ~PlayerController() = default;

		void Begin() override;

		void AddAxisMapping(const std::string& axisName, int key, float scale);
		void AddActionMapping(const std::string& actionName, const ActionMapping& key);

		/*������Ʈ�ѷ��� Ư�� ���� �Է��� �ް��մϴ�*/
		void Possess(std::shared_ptr<Pawn> pawn) override;
		bool OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

		void ProcessPitchInput(float value);
		void ProcessYawInput(float value);

		void SetInputMode(EInputMode inputMode) { m_InputMode = inputMode; }
		/*UI���� Game��带 Ư�� Ű�� ���Ͽ� ����ġ �� �� �ְ� �մϴ�. ����Ʈ ������ ���콺 Ű*/
		void EnableModeSwitching(bool bEnable, int64_t keyCode = Crystal::Mouse::Right);

		const std::map<int64_t, std::pair<std::string, float>>& GetAxisMap() const { return m_AxisMap; }
		const std::map<ActionMapping, std::string, ActionKeyCompare>& GetActionMap() const { return m_ActionMap; }

		uint8_t GetNetworkId() const { return m_NetworkId; }
		void SetNetworkId(uint8_t id) { m_NetworkId = id; }

		DirectX::XMFLOAT2 ProjectWorldToCameraSpace(const DirectX::XMFLOAT3& worldPosition);

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

		EInputMode m_InputMode = EInputMode::IM_Game;
		bool m_bIsSwitchableMode = false;

		uint8_t m_NetworkId = 0;
	};
}
