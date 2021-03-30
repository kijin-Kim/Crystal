#include <Crystal.h>
#include <Crystal/Core/EntryPoint.h>

#include "Crystal/GamePlay/Actors/Pawn.h"
#include "Crystal/GamePlay/Actors/CameraPawn.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"

#include "actors/TestPawn.h"
#include "actors/Kraken.h"
#include "actors\Aestroid.h"
#include "Crystal/Resources/ResourceManager.h"
#include "Crystal/GamePlay/Actors/SkyboxActor.h"

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
		Crystal::Application::Start();

		auto& resourceManager = Crystal::ResourceManager::Instance();

		resourceManager.LoadRenderableFromFile<Crystal::StaticMesh>("assets/models/SM_Frigate_BE2.fbx", "Frigate");


		/*Spawn된 Actor의 Ownership은 World에 있음*/
		TestPawn* testPawn = m_World->SpawnActor<TestPawn>();
		Crystal::SkyboxActor* skyboxActor = m_World->SpawnActor<Crystal::SkyboxActor>();
		
//		Kraken* kraken = m_World->SpawnActor<Kraken>();

		for (int i = -10; i < 10; i++)
		{
			Asteroid* asteroid = m_World->SpawnActor<Asteroid>();
			asteroid->SetPosition({ 3000.0f * i, 0.0f, 0.0f });
		}

		for (int i = -10; i < 10; i++)
		{
			RandomBox* randomBox = m_World->SpawnActor<RandomBox>();
			randomBox->SetPosition({ 3000.0f * i, 3000.0f, 0.0f });
		}


		/*Crystal::CameraPawn* cameraPawn = m_World->SpawnActor<Crystal::CameraPawn>();*/
		/*키바인딩*/
		auto& playerController = Crystal::ApplicationUtility::GetPlayerController();
		playerController.Possess(testPawn);
		playerController.AddAxisMapping("MoveForward", Crystal::Keyboard::W, 1.0f);
		playerController.AddAxisMapping("MoveForward", Crystal::Keyboard::S, -1.0f);
		playerController.AddAxisMapping("MoveRight", Crystal::Keyboard::D, 1.0f);
		playerController.AddAxisMapping("MoveRight", Crystal::Keyboard::A, -1.0f);
		playerController.AddAxisMapping("RollRight", Crystal::Keyboard::E, 1.0f);
		playerController.AddAxisMapping("RollRight", Crystal::Keyboard::Q, -1.0f);
		playerController.AddAxisMapping("MoveUp", Crystal::Keyboard::Space, 1.0f);
		playerController.AddAxisMapping("MoveUp", Crystal::Keyboard::LControl, -1.0f);
		playerController.AddAxisMapping("Turn", Crystal::Mouse::X, 1.0f);
		playerController.AddAxisMapping("LookUp", Crystal::Mouse::Y, -1.0f);
		playerController.EnableModeSwitching(true);

		Crystal::ActionMapping fireActionMapping = {};
		fireActionMapping.CrystalCode = Crystal::Mouse::Button::Left;
		fireActionMapping.bAltDown = false;
		fireActionMapping.bCtrlDown = false;
		fireActionMapping.bShiftDown = false;
		playerController.AddActionMapping("Fire", fireActionMapping);
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