#include <Crystal.h>
#include <Crystal/Core/EntryPoint.h>

#include "Crystal/GamePlay/Actors/Pawn.h"
#include "Crystal/GamePlay/Actors/CameraPawn.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"

#include "actors/TestPawn.h"
#include "actors/Kraken.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080


class GameApplication : public Crystal::Application
{
public:
	GameApplication(HINSTANCE hIsntance, int width, int height) : Crystal::Application(hIsntance, width, height)
	{
		/*디폴트 월드*/
		m_World = std::make_unique<Crystal::World>(nullptr);

	}

	void Start() override
	{
		/*Spawn된 Actor의 Ownership은 World에 있음*/
		for (int i = 0; i < 5; i++)
		{
			TestPawn* testPawn = m_World->SpawnActor<TestPawn>();
			testPawn->SetPosition({ 5.0f * i, 0.0f, 0.0f });
		}
			
//		Kraken* kraken = m_World->SpawnActor<Kraken>();

		Crystal::CameraPawn* cameraPawn = m_World->SpawnActor<Crystal::CameraPawn>();

		/*키바인딩*/
		auto& playerController = Crystal::ApplicationUtility::GetPlayerController();
		playerController.Possess(cameraPawn);
		playerController.AddAxisMapping("MoveForward", Crystal::Keyboard::W, 1.0f);
		playerController.AddAxisMapping("MoveForward", Crystal::Keyboard::S, -1.0f);
		playerController.AddAxisMapping("MoveRight", Crystal::Keyboard::D, 1.0f);
		playerController.AddAxisMapping("MoveRight", Crystal::Keyboard::A, -1.0f);
		playerController.AddAxisMapping("MoveUp", Crystal::Keyboard::PageUp, 1.0f);
		playerController.AddAxisMapping("MoveUp", Crystal::Keyboard::PageDown, -1.0f);

		playerController.AddAxisMapping("Turn", Crystal::Mouse::X, 1.0f);
		playerController.AddAxisMapping("LookUp", Crystal::Mouse::Y, 1.0f);
		playerController.EnableModeSwitching(true);

		Crystal::ActionMapping actionMapping = {};
		actionMapping.CrystalCode = Crystal::Mouse::Button::Left;
		actionMapping.bAltDown = false;
		actionMapping.bCtrlDown = false;
		actionMapping.bShiftDown = false;
		playerController.AddActionMapping("Fire", actionMapping);
	}

	void OnUpdate() override
	{
		m_MainTimer.Tick();
		m_World->Update(m_MainTimer.DeltaTime());
	}

private:
	Crystal::Timer m_MainTimer;
	std::unique_ptr<Crystal::World> m_World;

};

Register_Application(GameApplication, WINDOW_WIDTH, WINDOW_HEIGHT);