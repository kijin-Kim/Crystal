#include "cspch.h"
#include "PlayerController.h"

namespace Crystal {
	PlayerController::PlayerController()
	{
		/*���� �������̽��� ��ǲ ������Ʈ�� �����մϴ�.*/
		m_UserInterfaceInputComponent = std::make_unique<InputComponent>();


		m_MainComponent = CreateComponent<TransformComponent>("DefaultMainComponent");
	}

	void PlayerController::AddAxisMapping(const std::string& axisName, int key, float scale)
	{
		auto result = m_AxisMap.insert(std::make_pair(key, std::make_pair(axisName, scale)));
		CS_FATAL(result.second, "AxisMapping�� �����Ͽ����ϴ�.");
	}

	void PlayerController::AddActionMapping(const std::string& actionName, const ActionMapping& key)
	{
		auto result = m_ActionMap.insert(std::make_pair(key, actionName));
		CS_FATAL(result.second, "ActionMapping�� �����Ͽ����ϴ�.");
	}

	void PlayerController::Possess(std::shared_ptr<Pawn> pawn)
	{
		Controller::Possess(pawn);
		const auto camera = Cast<CameraComponent>(pawn->GetComponentByClass("CameraComponent"));
		if (!camera)
		{
			CS_WARN("Possess�ϴ� Pawn�� ī�޶� �������� �ʽ��ϴ�");
			return;
		}
		
		SetMainCamera(camera);
			
		m_GameInputComponent = std::make_unique<InputComponent>();
		m_GameInputComponent->BindCursor(true); // Ŀ���� ȭ�� �� ������ŵ�ϴ�.
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
			CS_FATAL(false, "��ȿ���� ���� ���� ����Դϴ�.");
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
		/*UI, Game ��ǲ�� ���� ��带 �ٲٴ� Action�� ���ε� �մϴ�.*/

		bool bIsSwitchableMode = bEnable;
		bool bWasSwitchableMode = m_bIsSwitchableMode;
		m_bIsSwitchableMode = bEnable;

		if (bIsSwitchableMode == bWasSwitchableMode)
			return;

		/*Switchable Mode�� ���� ���� UI ���� �����˴ϴ�.*/
		m_InputMode = EInputMode::IM_UI;

		ActionMapping actionKey = {};
		actionKey.CrystalCode = keyCode;
		actionKey.bAltDown = false;
		actionKey.bCtrlDown = false;
		actionKey.bShiftDown = false;
		AddActionMapping("UIToGameToUI", actionKey);

		/*���� ���� Switchable Mode�� �ƴϾ���, �̹��� Switchable ��尡 �ȴٸ�,*/
		if (!bWasSwitchableMode && bIsSwitchableMode)
		{
			if (!m_GameInputComponent)
				CS_FATAL(false, "���� ��带 ���� ��ǲ�� �غ�Ǿ� ���� �ʽ��ϴ�. ���� Pawn�� ���� ���ּ���.");

			m_UserInterfaceInputComponent->BindAction("UIToGameToUI", EKeyEvent::KE_Pressed, [this]() { SetInputMode(EInputMode::IM_Game); m_GameInputComponent->ReadyCursorBinding(); });
			m_GameInputComponent->BindAction("UIToGameToUI", EKeyEvent::KE_Released, [this]() { SetInputMode(EInputMode::IM_UI); });
		}
		/*���� ���� Switchable Mode����, �̹��� Switchable ��尡 �ȴٸ�,*/
		else if (bWasSwitchableMode && !bIsSwitchableMode)
		{
			m_UserInterfaceInputComponent->UnBindAction("UIToGameToUI", EKeyEvent::KE_Pressed);
			m_UserInterfaceInputComponent->UnBindAction("UIToGameToUI", EKeyEvent::KE_Released);
		}
	}
}