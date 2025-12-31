#include "cspch.h"
#include "PlayerController.h"

#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Scene.h"

namespace Crystal {
	PlayerController::PlayerController()
	{
		/*���� �������̽��� ��ǲ ������Ʈ�� �����մϴ�.*/
		m_UserInterfaceInputComponent = std::make_unique<InputComponent>();


		m_MainComponent = CreateComponent<TransformComponent>("DefaultMainComponent");
	}

	void PlayerController::Begin()
	{
		Actor::Begin();
		Cast<Level>(GetOuter())->AddPlayerController(Cast<PlayerController>(shared_from_this()));

		// Default Mapping
		AddAxisMapping("MoveForward", Crystal::Keyboard::W, 1.0f);
		AddAxisMapping("MoveForward", Crystal::Keyboard::S, -1.0f);
		AddAxisMapping("MoveRight", Crystal::Keyboard::D, 1.0f);
		AddAxisMapping("MoveRight", Crystal::Keyboard::A, -1.0f);
		AddAxisMapping("RollRight", Crystal::Keyboard::E, 1.0f);
		AddAxisMapping("RollRight", Crystal::Keyboard::Q, -1.0f);
		AddAxisMapping("MoveUp", Crystal::Keyboard::Space, 1.0f);
		AddAxisMapping("MoveUp", Crystal::Keyboard::LControl, -1.0f);
		AddAxisMapping("Turn", Crystal::Mouse::X, 1.0f);
		AddAxisMapping("LookUp", Crystal::Mouse::Y, -1.0f);

		Crystal::ActionMapping fireActionMapping = {};
		fireActionMapping.CrystalCode = Crystal::Mouse::Button::Left;
		fireActionMapping.bAltDown = false;
		fireActionMapping.bCtrlDown = false;
		fireActionMapping.bShiftDown = false;
		AddActionMapping("Fire", fireActionMapping);

		Crystal::ActionMapping usePowerItemActionMapping = {};
		usePowerItemActionMapping.CrystalCode = Crystal::Keyboard::Num1;
		usePowerItemActionMapping.bAltDown = false;
		usePowerItemActionMapping.bCtrlDown = false;
		usePowerItemActionMapping.bShiftDown = false;
		AddActionMapping("UsePowerItem", usePowerItemActionMapping);

		Crystal::ActionMapping useHealItemActionMapping = {};
		useHealItemActionMapping.CrystalCode = Crystal::Keyboard::Num2;
		useHealItemActionMapping.bAltDown = false;
		useHealItemActionMapping.bCtrlDown = false;
		useHealItemActionMapping.bShiftDown = false;
		AddActionMapping("UseHealItem", useHealItemActionMapping);

		Crystal::ActionMapping useShieldItemActionMapping = {};
		useShieldItemActionMapping.CrystalCode = Crystal::Keyboard::Num3;
		useShieldItemActionMapping.bAltDown = false;
		useShieldItemActionMapping.bCtrlDown = false;
		useShieldItemActionMapping.bShiftDown = false;
		AddActionMapping("UseShieldItem", useShieldItemActionMapping);

		Crystal::ActionMapping toggleShowDebugCollisionActionMapping = {};
		toggleShowDebugCollisionActionMapping.CrystalCode = Crystal::Keyboard::Num9;
		toggleShowDebugCollisionActionMapping.bAltDown = false;
		toggleShowDebugCollisionActionMapping.bCtrlDown = false;
		toggleShowDebugCollisionActionMapping.bShiftDown = false;
		AddActionMapping("ShowDebugCollision", toggleShowDebugCollisionActionMapping);

		Crystal::ActionMapping toggleShowDebugAIActionMapping = {};
		toggleShowDebugAIActionMapping.CrystalCode = Crystal::Keyboard::Num0;
		toggleShowDebugAIActionMapping.bAltDown = false;
		toggleShowDebugAIActionMapping.bCtrlDown = false;
		toggleShowDebugAIActionMapping.bShiftDown = false;
		AddActionMapping("ShowDebugAI", toggleShowDebugAIActionMapping);

		Crystal::ActionMapping toggleHideDebugCollisionActionMapping = {};
		toggleHideDebugCollisionActionMapping.CrystalCode = Crystal::Keyboard::O;
		toggleHideDebugCollisionActionMapping.bAltDown = false;
		toggleHideDebugCollisionActionMapping.bCtrlDown = false;
		toggleHideDebugCollisionActionMapping.bShiftDown = false;
		AddActionMapping("HideDebugCollision", toggleHideDebugCollisionActionMapping);

		Crystal::ActionMapping toggleHideDebugAIActionMapping = {};
		toggleHideDebugAIActionMapping.CrystalCode = Crystal::Keyboard::P;
		toggleHideDebugAIActionMapping.bAltDown = false;
		toggleHideDebugAIActionMapping.bCtrlDown = false;
		toggleHideDebugAIActionMapping.bShiftDown = false;
		AddActionMapping("HideDebugAI", toggleHideDebugAIActionMapping);

		Crystal::ActionMapping openNewLevelActionMapping = {};
		openNewLevelActionMapping.CrystalCode = Crystal::Keyboard::Enter;
		openNewLevelActionMapping.bAltDown = false;
		openNewLevelActionMapping.bCtrlDown = false;
		openNewLevelActionMapping.bShiftDown = false;
		AddActionMapping("OpenNewLevel", openNewLevelActionMapping);

		Crystal::ActionMapping interactMapping = {};
		interactMapping.CrystalCode = Crystal::Keyboard::V;
		interactMapping.bAltDown = false;
		interactMapping.bCtrlDown = false;
		interactMapping.bShiftDown = false;
		AddActionMapping("Interact", interactMapping);

	}

	void PlayerController::AddAxisMapping(const std::string& axisName, int key, float scale)
	{
		auto result = m_AxisMap.insert(std::make_pair(key, std::make_pair(axisName, scale)));
	}

	void PlayerController::AddActionMapping(const std::string& actionName, const ActionMapping& key)
	{
		auto result = m_ActionMap.insert(std::make_pair(key, actionName));
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

		auto level = Cast<Level>(GetOuter());
		auto& scene = level->GetScene();

		scene->SetMainCamera(camera);

		m_GameInputComponent = std::make_unique<InputComponent>();
		m_GameInputComponent->BindCursor(true); // Ŀ���� ȭ�� �� ������ŵ�ϴ�.
		m_GameInputComponent->ShowCursor(false);
		m_GameInputComponent->SetOuter(pawn);
		m_GameInputComponent->ReadyCursorBinding();

		m_UserInterfaceInputComponent->SetOuter(pawn);


		ActionMapping actionKey = {};
		actionKey.CrystalCode = Mouse::Button::Left;
		actionKey.bAltDown = false;
		actionKey.bCtrlDown = false;
		actionKey.bShiftDown = false;
		AddActionMapping("MouseClickAction", actionKey);


		m_UserInterfaceInputComponent->BindAction("MouseClickAction", EKeyEvent::KE_Pressed, [this]()
		{
			auto button = m_CurrentButton.lock();
			if (button)
			{
				m_PressedButton = button;
				button->OnButtonPressed();
			}
		});

		m_UserInterfaceInputComponent->BindAction("MouseClickAction", EKeyEvent::KE_Released, [this]()
		{
			auto pressedButton = m_PressedButton.lock();
			auto currentButton = m_CurrentButton.lock();
			if (pressedButton && currentButton)
			{
				if(m_bCurrentButtonIsHovered && pressedButton == currentButton)
				{
					pressedButton->OnButtonClicked();
				}

				m_CurrentButton = {};

				pressedButton->OnButtonReleased();
			}
		});


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

	void PlayerController::SetInputMode(EInputMode inputMode)
	{
		switch (inputMode)
		{
		case EInputMode::IM_Game:
			m_GameInputComponent->ReadyCursorBinding();
			break;
		case EInputMode::IM_UI: break;
		default: ;
		}
		m_InputMode = inputMode;
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

			m_UserInterfaceInputComponent->BindAction("UIToGameToUI", EKeyEvent::KE_Pressed, [this]() { SetInputMode(EInputMode::IM_Game); });
			m_GameInputComponent->BindAction("UIToGameToUI", EKeyEvent::KE_Pressed, [this]() { SetInputMode(EInputMode::IM_UI); });
		}
			/*���� ���� Switchable Mode����, �̹��� Switchable ��尡 �ȴٸ�,*/
		else if (bWasSwitchableMode && !bIsSwitchableMode)
		{
			m_UserInterfaceInputComponent->UnBindAction("UIToGameToUI", EKeyEvent::KE_Pressed);
			m_UserInterfaceInputComponent->UnBindAction("UIToGameToUI", EKeyEvent::KE_Pressed);
		}
	}

	void PlayerController::SetCurrentButton(Crystal::Weak<ButtonComponent> button)
	{
		m_CurrentButton = button;
	}

	Weak<ButtonComponent> PlayerController::GetCurrentButton() const
	{
		return m_CurrentButton;
	}

	DirectX::XMFLOAT2 PlayerController::ProjectWorldToCameraSpace(const DirectX::XMFLOAT3& worldPosition)
	{
		auto level = Cast<Level>(GetLevel());
		if (!level)
		{
			return {0.0f, 0.0f};
		}

		auto playerPawn = Cast<Pawn>(level->GetPlayerPawn());
		if (!playerPawn)
		{
			return {0.0f, 0.0f};
		}

		auto cameraComponent = Cast<CameraComponent>(playerPawn->GetComponentByClass("CameraComponent"));
		if (!cameraComponent)
		{
			return {0.0f, 0.0f};
		}

		return cameraComponent->ProjectWorldToCameraSpace(worldPosition);
	}
}


