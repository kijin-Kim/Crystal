#include "cspch.h"
#include "PlayerController.h"

namespace Crystal {
	PlayerController::PlayerController()
	{
		/*유저 인터페이스용 인풋 컴포넌트를 설정합니다.*/
		m_UserInterfaceInputComponent = std::make_unique<InputComponent>();


		m_MainComponent = CreateComponent<TransformComponent>("DefaultMainComponent");
	}

	void PlayerController::AddAxisMapping(const std::string& axisName, int key, float scale)
	{
		auto result = m_AxisMap.insert(std::make_pair(key, std::make_pair(axisName, scale)));
		CS_FATAL(result.second, "AxisMapping에 실패하였습니다.");
	}

	void PlayerController::AddActionMapping(const std::string& actionName, const ActionMapping& key)
	{
		auto result = m_ActionMap.insert(std::make_pair(key, actionName));
		CS_FATAL(result.second, "ActionMapping에 실패하였습니다.");
	}

	void PlayerController::Possess(std::shared_ptr<Pawn> pawn)
	{
		Controller::Possess(pawn);
		const auto camera = Cast<CameraComponent>(pawn->GetComponentByClass("CameraComponent"));
		if (!camera)
		{
			CS_WARN("Possess하는 Pawn에 카메라가 존재하지 않습니다");
			return;
		}
		
		SetMainCamera(camera);
			
		m_GameInputComponent = std::make_unique<InputComponent>();
		m_GameInputComponent->BindCursor(true); // 커서를 화면 상에 고정시킵니다.
		m_GameInputComponent->ShowCursor(false);
		m_GameInputComponent->SetOuter(pawn);
		
		m_UserInterfaceInputComponent->SetOuter(pawn);
		pawn->SetupInputComponent(m_GameInputComponent.get());
	}

	bool PlayerController::OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{		
		switch (m_InputMode)
		{
		case Crystal::EInputMode::IM_Game:
		{
			if (m_GameInputComponent)
			{
				return m_GameInputComponent->ProcessInputEvent(hWnd, uMsg, wParam, lParam);
			}
			return m_UserInterfaceInputComponent->ProcessInputEvent(hWnd, uMsg, wParam, lParam);
		}
		case Crystal::EInputMode::IM_UI:
			return m_UserInterfaceInputComponent->ProcessInputEvent(hWnd, uMsg, wParam, lParam);
		default:
			CS_FATAL(false, "유효하지 않은 게임 모드입니다.");
			break;
		}
		return false;
	}

	void PlayerController::ProcessPitchInput(float value)
	{
	}

	void PlayerController::ProcessYawInput(float value)
	{
	}

	void PlayerController::EnableModeSwitching(bool bEnable, int64_t keyCode /*= Crystal::Mouse::Right*/)
	{
		/*UI, Game 인풋에 각각 모드를 바꾸는 Action을 바인드 합니다.*/

		bool bIsSwitchableMode = bEnable;
		bool bWasSwitchableMode = m_bIsSwitchableMode;
		m_bIsSwitchableMode = bEnable;

		if (bIsSwitchableMode == bWasSwitchableMode)
			return;

		/*Switchable Mode에 들어가면 최초 UI 모드로 설정됩니다.*/
		m_InputMode = EInputMode::IM_UI;

		ActionMapping actionKey = {};
		actionKey.CrystalCode = keyCode;
		actionKey.bAltDown = false;
		actionKey.bCtrlDown = false;
		actionKey.bShiftDown = false;
		AddActionMapping("UIToGameToUI", actionKey);

		/*만약 원래 Switchable Mode가 아니었고, 이번에 Switchable 모드가 된다면,*/
		if (!bWasSwitchableMode && bIsSwitchableMode)
		{
			if (!m_GameInputComponent)
				CS_FATAL(false, "게임 모드를 위한 인풋이 준비되어 있지 않습니다. 먼저 Pawn을 빙의 해주세요.");

			m_UserInterfaceInputComponent->BindAction("UIToGameToUI", EKeyEvent::KE_Pressed, [this]() { SetInputMode(EInputMode::IM_Game); m_GameInputComponent->ReadyCursorBinding(); });
			m_GameInputComponent->BindAction("UIToGameToUI", EKeyEvent::KE_Released, [this]() { SetInputMode(EInputMode::IM_UI); });
		}
		/*만약 원래 Switchable Mode였고, 이번에 Switchable 모드가 된다면,*/
		else if (bWasSwitchableMode && !bIsSwitchableMode)
		{
			m_UserInterfaceInputComponent->UnBindAction("UIToGameToUI", EKeyEvent::KE_Pressed);
			m_UserInterfaceInputComponent->UnBindAction("UIToGameToUI", EKeyEvent::KE_Released);
		}
	}
}