#include <Crystal.h>
#include <Crystal/Core/EntryPoint.h>

#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/GamePlay/Objects/Actors/CameraPawn.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"

#include "actors/TestPawn.h"
#include "actors/Asteroid.h"
#include "Crystal/Resources/ResourceManager.h"
#include "actors/Sun.h"
#include "Crystal/GamePlay/Objects/Actors/GameMode.h"
#include "Crystal/GamePlay/Objects/Actors/ParticleActor.h"
#include "Crystal/GamePlay/Objects/Actors/PlayerStartActor.h"

#include <sstream>



class GameApplication : public Crystal::Application
{
public:
	GameApplication(int width, int height) : Crystal::Application(width, height)
	{
	}

	void Start() override
	{
		Crystal::Application::Start();
		auto& resourceManager = Crystal::ResourceManager::Instance();

		//============ Load Renderables =======================================
		resourceManager.CreateRenderableFromFile<Crystal::StaticMesh>("assets/models/SM_Frigate_BE2.fbx", "Frigate");
		resourceManager.CreateRenderableFromFile<
			Crystal::StaticMesh>("assets/models/Asteroid_1.fbx", "Asteroid_Mesh_1");

		//============ Load Textures ==========================================

		//=== Frigate Textures =================================
		auto frigateAlbedo = resourceManager.CreateTextureFromFile(
			"assets/textures/T_Frigate_BE2/T_M_SM_Frigate_BE2_MI_Frigate_BE2_White_BaseColor.tga",
			"Frigate_Albedo").lock();
		if(frigateAlbedo)
		{
			frigateAlbedo->CreateShaderResourceView(frigateAlbedo->GetResource()->GetDesc().Format,
				D3D12_SRV_DIMENSION_TEXTURE2D);
		}
		


		auto frigateMetallic = resourceManager.CreateTextureFromFile(
			"assets/textures/T_Frigate_BE2/T_Frigate_BE2_Metallic.tga", "Frigate_Metallic").lock();

		if(frigateMetallic)
		{
			frigateMetallic->CreateShaderResourceView(frigateMetallic->GetResource()->GetDesc().Format,
				D3D12_SRV_DIMENSION_TEXTURE2D);
		}
	

		auto frigateRoughness = resourceManager.CreateTextureFromFile(
			"assets/textures/T_Frigate_BE2/T_Frigate_BE2_Roughness.tga", "Frigate_Roughness").lock();

		if(frigateRoughness)
		{
			frigateRoughness->CreateShaderResourceView(frigateRoughness->GetResource()->GetDesc().Format,
				D3D12_SRV_DIMENSION_TEXTURE2D);
		}
		

		auto frigateNormal = resourceManager.CreateTextureFromFile(
			"assets/textures/T_Frigate_BE2/T_Frigate_BE2_Norm.tga", "Frigate_Normal").lock();

		if(frigateNormal)
		{
			frigateNormal->CreateShaderResourceView(frigateNormal->GetResource()->GetDesc().Format,
				D3D12_SRV_DIMENSION_TEXTURE2D);
		}
		

		//=== Asteroid Textures =================================
		auto asteroidBlueAlbedo = resourceManager.CreateTextureFromFile(
			"assets/textures/Asteroid/AstOre_1_A.tga", "Asteroid_Blue_Albedo").lock();

		if(asteroidBlueAlbedo)
		{
			asteroidBlueAlbedo->CreateShaderResourceView(asteroidBlueAlbedo->GetResource()->GetDesc().Format,
				D3D12_SRV_DIMENSION_TEXTURE2D);
		}
		

		auto asteroidBlueMetallic = resourceManager.CreateTextureFromFile(
			"assets/textures/Asteroid/AstOre_1_M.tga", "Asteroid_Blue_Metallic").lock();
		if(asteroidBlueMetallic)
		{
			asteroidBlueMetallic->CreateShaderResourceView(asteroidBlueMetallic->GetResource()->GetDesc().Format,
				D3D12_SRV_DIMENSION_TEXTURE2D);
		}
		

		auto asteroidBlueRoughness = resourceManager.CreateTextureFromFile(
			"assets/textures/Asteroid/AstOre_1_R.tga", "Asteroid_Blue_Roughness").lock();
		if(asteroidBlueRoughness)
		{
			asteroidBlueRoughness->CreateShaderResourceView(asteroidBlueRoughness->GetResource()->GetDesc().Format,
				D3D12_SRV_DIMENSION_TEXTURE2D);
		}
		

		auto asteroidBlueNormal = resourceManager.CreateTextureFromFile(
			"assets/textures/Asteroid/AstOre_1_N.tga", "Asteroid_Blue_Normal").lock();
		if(asteroidBlueNormal)
		{
			asteroidBlueNormal->CreateShaderResourceView(asteroidBlueNormal->GetResource()->GetDesc().Format,
				D3D12_SRV_DIMENSION_TEXTURE2D);
		}
		

		auto asteroidBlueEmissive = resourceManager.CreateTextureFromFile(
			"assets/textures/Asteroid/AstOre_1_E.tga", "Asteroid_Blue_Emissive").lock();
		if(asteroidBlueEmissive)
		{
			asteroidBlueEmissive->CreateShaderResourceView(asteroidBlueEmissive->GetResource()->GetDesc().Format,
				D3D12_SRV_DIMENSION_TEXTURE2D);
		}
		


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


		m_World->SetCurrentLevelByName("DefaultLevel");

		//auto gameMode = m_World->SpawnActor<Crystal::GameMode>({ "FlightGameMode" }).lock();
		//gameMode->m_DefaultActorClass = std::make_unique<Crystal::ActorClassOf<TestPawn>>();

		
		/*Spawn된 Actor의 Ownership은 World에 있음*/




		auto sun = m_World->SpawnActor<Sun>({"Sun"}).lock();
		sun->SetPosition({0.0f, 200000.0f, 200000.0f});
		auto sunMesh = Crystal::Cast<Crystal::StaticMeshComponent>(sun->GetComponentByClass("StaticMeshComponent"));

		sunMesh->SetRenderable(resourceManager.GetRenderable("Sphere"));
		auto sunMat = sunMesh->GetMaterial(0);
		sunMat->EmissiveColor = DirectX::XMFLOAT3(1.0f * 3.0f, 1.0f * 3.0f, 0.4f * 3.0f);



		/*Kraken* kraken = m_World->SpawnActor<Kraken>("Kraken");*/

		for (int i = 0; i < 0; i++)
		{
			for (int j = 0; j < 0; j++)
			{
				for (int k = 0; k < 0; k++)
				{
					//test

					auto asteroid = m_World->SpawnActor<Asteroid>({}).lock();
					asteroid->SetPosition({1000.0f * i, 1000.0f * j, 1000.0f * k});

					auto staticMeshComponent = Crystal::Cast<Crystal::StaticMeshComponent>(
						asteroid->GetComponentByName("StaticMeshComponent"));
					staticMeshComponent->SetRenderable(resourceManager.GetRenderable("Asteroid_Mesh_1"));
					auto& materials = staticMeshComponent->GetMaterials();
					auto pbrMat = materials[0].get();
					pbrMat->AlbedoTexture = resourceManager.GetTexture("Asteroid_Blue_Albedo");
					pbrMat->MetallicTexture = resourceManager.GetTexture("Asteroid_Blue_Metallic");
					pbrMat->RoughnessTexture = resourceManager.GetTexture("Asteroid_Blue_Roughness");
					pbrMat->NormalTexture = resourceManager.GetTexture("Asteroid_Blue_Normal");
					pbrMat->EmissiveTexture = resourceManager.GetTexture("Asteroid_Blue_Emissive");

				}
			}
		}


		auto particleActor = m_World->SpawnActor<Crystal::ParticleActor>({}).lock();

		auto particleComponent = Crystal::Cast<Crystal::ParticleComponent>(
			particleActor->GetComponentByClass("ParticleComponent"));
		particleComponent->SetRenderable(resourceManager.GetRenderable("3DPlaneQuadMesh"));
		auto& particleMats = particleComponent->GetMaterials();
		auto particleMat = particleMats[0].get();
		particleMat->EmissiveTexture = resourceManager.GetTexture("Asteroid_Blue_Albedo");

		

		auto playerStart = m_World->SpawnActor<Crystal::PlayerStartActor>({"1"}).lock();
		playerStart->SetPosition({ 0.0f, 0.0f, -2000.0f });

		m_World->GetCurrentLevel()->OnClientConnect();

	}

	void Update() override
	{
		m_MainTimer.Tick();
		m_World->Update(m_MainTimer.GetDeltaSeconds());
	}

private:
	Crystal::Timer m_MainTimer;
};

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080


Register_Application(GameApplication, WINDOW_WIDTH, WINDOW_HEIGHT);
