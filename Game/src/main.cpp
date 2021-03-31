#include <Crystal.h>
#include <Crystal/Core/EntryPoint.h>

#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/GamePlay/Objects/Actors/CameraPawn.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"

#include "actors/TestPawn.h"
#include "actors/Kraken.h"
#include "actors/Aestroid.h"
#include "Crystal/Resources/ResourceManager.h"
#include "Crystal/GamePlay/Objects/Actors/SkyboxActor.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

class GameApplication : public Crystal::Application
{
public:
	GameApplication(HINSTANCE hIsntance, int width, int height) : Crystal::Application(hIsntance, width, height)
	{
	}

	void Start() override
	{
		Crystal::Application::Start();

		auto& resourceManager = Crystal::ResourceManager::Instance();
		
		//============ Load Renderables =======================================
		resourceManager.CreateRenderableFromFile<Crystal::StaticMesh>("assets/models/SM_Frigate_BE2.fbx", "Frigate");

		//============ Load Textures ==========================================
		auto frigateAlbedoWeak = resourceManager.CreateTextureFromFile(
			"assets/textures/T_Frigate_BE2/T_M_SM_Frigate_BE2_MI_Frigate_BE2_White_BaseColor.tga", "Frigate_Albedo");
		if (auto frigateAlbedo = frigateAlbedoWeak.lock())
		{
			frigateAlbedo->CreateShaderResourceView(frigateAlbedo->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		}
			
		auto frigateMetallicWeak = resourceManager.CreateTextureFromFile(
			"assets/textures/T_Frigate_BE2/T_Frigate_BE2_Metallic.tga", "Frigate_Metallic");
		if (auto frigateMetallic = frigateMetallicWeak.lock())
		{
			frigateMetallic->CreateShaderResourceView(frigateMetallic->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		}

		auto frigateRoughnessWeak = resourceManager.CreateTextureFromFile(
			"assets/textures/T_Frigate_BE2/T_Frigate_BE2_Roughness.tga", "Frigate_Roughness");
		if (auto frigateRoughness = frigateRoughnessWeak.lock())
		{
			frigateRoughness->CreateShaderResourceView(frigateRoughness->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		}
			
		auto frigateNormalWeak = resourceManager.CreateTextureFromFile(
			"assets/textures/T_Frigate_BE2/T_Frigate_BE2_Norm.tga", "Frigate_Normal");
		if (auto frigateNormal = frigateNormalWeak.lock())
		{
			frigateNormal->CreateShaderResourceView(frigateNormal->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		}
		//=====================================================================


		auto defaultLevel = m_World->GetLevelByName("DefaultLevel");

		/*Spawn된 Actor의 Ownership은 World에 있음*/
		TestPawn* testPawn = defaultLevel->SpawnActor<TestPawn>("TestPawn");
		Crystal::SkyboxActor* skyboxActor = defaultLevel->SpawnActor<Crystal::SkyboxActor>("SkyboxActor");

		//Kraken* kraken = defaultLevel->SpawnActor<Kraken>("Kraken");

		for (int i = -10; i < 10; i++)
		{
			Asteroid* asteroid = defaultLevel->SpawnActor<Asteroid>();
			asteroid->SetPosition({ 3000.0f * i, 0.0f, 0.0f });
		}

		for (int i = -10; i < 10; i++)
		{
			RandomBox* randomBox = defaultLevel->SpawnActor<RandomBox>();
			randomBox->SetPosition({ 3000.0f * i, 3000.0f, 0.0f });
		}

		/*Crystal::CameraPawn* cameraPawn = defaultLevel->SpawnActor<Crystal::CameraPawn>();*/
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
};

Register_Application(GameApplication, WINDOW_WIDTH, WINDOW_HEIGHT);