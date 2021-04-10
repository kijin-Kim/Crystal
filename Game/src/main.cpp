#include <Crystal.h>
#include <Crystal/Core/EntryPoint.h>

#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/GamePlay/Objects/Actors/CameraPawn.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"

#include "actors/TestPawn.h"
#include "actors/Kraken.h"
#include "actors/Asteroid.h"
#include "Crystal/Resources/ResourceManager.h"
#include "Crystal/GamePlay/Objects/Actors/SkyboxActor.h"
#include "actors/Sun.h"
#include "Crystal/GamePlay/Objects/Actors/TonemappingActor.h"

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
		resourceManager.CreateRenderableFromFile<Crystal::StaticMesh>("assets/models/Asteroid_1.fbx", "Asteroid_Mesh_1");

		//============ Load Textures ==========================================

		//=== Frigate Textures =================================
		auto frigateAlbedo = resourceManager.CreateTextureFromFile(
			"assets/textures/T_Frigate_BE2/T_M_SM_Frigate_BE2_MI_Frigate_BE2_White_BaseColor.tga", "Frigate_Albedo").lock();
		frigateAlbedo->CreateShaderResourceView(frigateAlbedo->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		


		auto frigateMetallic = resourceManager.CreateTextureFromFile(
			"assets/textures/T_Frigate_BE2/T_Frigate_BE2_Metallic.tga", "Frigate_Metallic").lock();
		frigateMetallic->CreateShaderResourceView(frigateMetallic->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);

		auto frigateRoughness = resourceManager.CreateTextureFromFile(
			"assets/textures/T_Frigate_BE2/T_Frigate_BE2_Roughness.tga", "Frigate_Roughness").lock();
		frigateRoughness->CreateShaderResourceView(frigateRoughness->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
			
		auto frigateNormal = resourceManager.CreateTextureFromFile(
			"assets/textures/T_Frigate_BE2/T_Frigate_BE2_Norm.tga", "Frigate_Normal").lock();
		frigateNormal->CreateShaderResourceView(frigateNormal->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);

		//=== Asteroid Textures =================================
		auto asteroidBlueAlbedo = resourceManager.CreateTextureFromFile(
			"assets/textures/Asteroid/AstOre_1_A.tga", "Asteroid_Blue_Albedo").lock();
		asteroidBlueAlbedo->CreateShaderResourceView(asteroidBlueAlbedo->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);

		auto asteroidBlueMetallic = resourceManager.CreateTextureFromFile(
			"assets/textures/Asteroid/AstOre_1_M.tga", "Asteroid_Blue_Metallic").lock();
		asteroidBlueMetallic->CreateShaderResourceView(asteroidBlueMetallic->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);

		auto asteroidBlueRoughness = resourceManager.CreateTextureFromFile(
			"assets/textures/Asteroid/AstOre_1_R.tga", "Asteroid_Blue_Roughness").lock();
		asteroidBlueRoughness->CreateShaderResourceView(asteroidBlueRoughness->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);

		auto asteroidBlueNormal = resourceManager.CreateTextureFromFile(
			"assets/textures/Asteroid/AstOre_1_N.tga", "Asteroid_Blue_Normal").lock();
		asteroidBlueNormal->CreateShaderResourceView(asteroidBlueNormal->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);

		auto asteroidBlueEmissive = resourceManager.CreateTextureFromFile(
			"assets/textures/Asteroid/AstOre_1_E.tga", "Asteroid_Blue_Emissive").lock();
		asteroidBlueEmissive->CreateShaderResourceView(asteroidBlueEmissive->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);


		//=== Kraken Textures =================================

		//=== body ========
		//auto krakenBodyAlbedo = resourceManager.CreateTextureFromFile(
		//	"assets/textures/Kraken/Tex_KRAKEN_BODY_BaseColor.tga", "Kraken_Body_Albedo").lock();
		//krakenBodyAlbedo->CreateShaderResourceView(krakenBodyAlbedo->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);

		//auto krakenBodyRoughness = resourceManager.CreateTextureFromFile(
		//	"assets/textures/Kraken/T_M_KRAKEN_Mat_KRAKEN_MAIN_BODY_Roughness.tga", "Kraken_Body_Roughness").lock();
		//krakenBodyRoughness->CreateShaderResourceView(krakenBodyRoughness->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);

		//auto krakenBodyNormal = resourceManager.CreateTextureFromFile(
		//	"assets/textures/Kraken/Tex_KRAKEN_BODY_NRM.tga", "Kraken_Body_Normal").lock();
		//krakenBodyNormal->CreateShaderResourceView(krakenBodyNormal->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);

		////=== tentacle =====
		//auto krakenTentacleAlbedo = resourceManager.CreateTextureFromFile(
		//	"assets/textures/Kraken/Tex_KRAKEN_LEG_TENTACLE_BaseColor.tga", "Kraken_Tentacle_Albedo").lock();
		//krakenTentacleAlbedo->CreateShaderResourceView(krakenTentacleAlbedo->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		//
		//auto krakenTentacleRoughness = resourceManager.CreateTextureFromFile(
		//	"assets/textures/Kraken/T_M_KRAKEN_Mat_TENTACLES_LEGS_CLAWS_Roughness.tga", "Kraken_Tentacle_Roughness").lock();
		//krakenTentacleRoughness->CreateShaderResourceView(krakenTentacleRoughness->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);

		//auto krakenTentacleNormal = resourceManager.CreateTextureFromFile(
		//	"assets/textures/Kraken/Tex_KRAKEN_LEG_TENTACLE_CLAW_NRM.tga", "Kraken_Tentacle_Normal").lock();
		//krakenTentacleNormal->CreateShaderResourceView(krakenTentacleNormal->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		//=====================================================================


		auto defaultLevel = m_World->GetLevelByName("DefaultLevel");

		/*Spawn된 Actor의 Ownership은 World에 있음*/
		TestPawn* testPawn = defaultLevel->SpawnActor<TestPawn>("TestPawn");
		testPawn->SetPosition({ 0.0f, 0.0f, -2000.0f });

		Sun* sun = defaultLevel->SpawnActor<Sun>("Sun");
		sun->SetPosition({ 0.0f, 200000.0f, 200000.0f });
		sun = defaultLevel->SpawnActor<Sun>("Sun2");
		sun->SetPosition({ 0.0f, -200000.0f, -200000.0f });

		/*sun = defaultLevel->SpawnActor<Sun>("Sun");
		sun->SetPosition({ -10000.0f, 0.0f, 0.0f });

		sun = defaultLevel->SpawnActor<Sun>("Sun");
		sun->SetPosition({ +10000.0f, 0.0f, 0.0f });*/


		Crystal::SkyboxActor* skyboxActor = defaultLevel->SpawnActor<Crystal::SkyboxActor>("SkyboxActor");
		Crystal::TonemappingActor* tonemappingActor = defaultLevel->SpawnActor<Crystal::TonemappingActor>("TonemappingActor");

		/*Kraken* kraken = defaultLevel->SpawnActor<Kraken>("Kraken");*/

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 1; j++)
			{
				Asteroid* asteroid = defaultLevel->SpawnActor<Asteroid>();
				asteroid->SetPosition({ 1000.0f * i, 1000.0f * j, 0.0f });
			}
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

	void Update() override
	{
		m_MainTimer.Tick();
		m_World->Update(m_MainTimer.DeltaTime());
	}

private:
	Crystal::Timer m_MainTimer;
};

Register_Application(GameApplication, WINDOW_WIDTH, WINDOW_HEIGHT);