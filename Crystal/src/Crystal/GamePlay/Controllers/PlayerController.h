#pragma once
#include <map>
#include "Controller.h"
#include "Crystal/GamePlay/Components/ButtonComponent.h"
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
		// 게임 만
		IM_UI,
		// UI 만
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

		/*게임컨트롤러가 특정 폰의 입력을 받게합니다*/
		void Possess(std::shared_ptr<Pawn> pawn) override;
		bool OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

		void ProcessPitchInput(float value);
		void ProcessYawInput(float value);

		void SetInputMode(EInputMode inputMode);
		EInputMode GetInputMode() const { return m_InputMode; }
		/*UI모드와 Game모드를 특정 키를 통하여 스위치 할 수 있게 합니다. 디폴트 오른쪽 마우스 키*/
		void EnableModeSwitching(bool bEnable, int64_t keyCode = Crystal::Mouse::Right);

		const std::multimap<int64_t, std::pair<std::string, float>>& GetAxisMap() const { return m_AxisMap; }
		const std::multimap<ActionMapping, std::string, ActionKeyCompare>& GetActionMap() const { return m_ActionMap; }

		uint8_t GetNetworkId() const { return m_NetworkId; }
		void SetNetworkId(uint8_t id) { m_NetworkId = id; }


		void SetCurrentButton(Crystal::Weak<ButtonComponent> button);
		void SetCurrentButtonIsHovered(bool bIsHovered) { m_bCurrentButtonIsHovered = bIsHovered; }

		Weak<ButtonComponent> GetCurrentButton() const;
		
		DirectX::XMFLOAT2 ProjectWorldToCameraSpace(const DirectX::XMFLOAT3& worldPosition);

		STATIC_TYPE_IMPLE(PlayerController)
	private:
		/*유저 인터페이스 인풋*/
		std::unique_ptr<InputComponent> m_UserInterfaceInputComponent = nullptr;
		/*게임모드 인풋*/
		std::unique_ptr<InputComponent> m_GameInputComponent = nullptr;
		/* KeyCode, AxisName, Scale */
		std::multimap<int64_t, std::pair<std::string, float>> m_AxisMap;
		/* KeyCode, ActionName */
		std::multimap<ActionMapping, std::string, ActionKeyCompare> m_ActionMap;

		EInputMode m_InputMode = EInputMode::IM_Game;
		bool m_bIsSwitchableMode = false;

		uint8_t m_NetworkId = 0;

		Weak<ButtonComponent> m_CurrentButton;
		Weak<ButtonComponent> m_PressedButton;
		bool m_bCurrentButtonIsHovered;
		
	};
}
