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
#include <boost/functional/hash.hpp>

#include "actors/Kraken.h"


class GameApplication : public Crystal::Application
{
public:
	GameApplication(int width, int height) : Crystal::Application(width, height)
	{
	}

	void Start() override
	{
		Crystal::Application::Start();

#if defined(CS_NM_STANDALONE) || defined(CS_NM_DEDICATED)

		auto& resourceManager = Crystal::ResourceManager::Instance();

		//============ Load Renderables =======================================
		resourceManager.CreateRenderableFromFile<Crystal::StaticMesh>("Frigate", "assets/models/SM_Frigate_BE2.fbx");
		resourceManager.CreateRenderableFromFile<
			Crystal::StaticMesh>("Asteroid_Mesh_1", "assets/models/Asteroid_1.fbx");

		resourceManager.CreateRenderableFromFile<
			Crystal::SkeletalMesh>("Kraken", "assets/models/KRAKEN.fbx",
			                       "assets/models/KRAKEN_idle.fbx");


		//============ Load Textures ==========================================

		//=== Frigate Textures =================================
		auto frigateAlbedo = resourceManager.CreateTextureFromFile(
			"assets/textures/T_Frigate_BE2/T_M_SM_Frigate_BE2_MI_Frigate_BE2_White_BaseColor.tga",
			"Frigate_Albedo").lock();
		if (frigateAlbedo)
		{
			frigateAlbedo->CreateShaderResourceView(frigateAlbedo->GetResource()->GetDesc().Format,
			                                        D3D12_SRV_DIMENSION_TEXTURE2D);
		}


		auto frigateMetallic = resourceManager.CreateTextureFromFile(
			"assets/textures/T_Frigate_BE2/T_Frigate_BE2_Metallic.tga", "Frigate_Metallic").lock();

		if (frigateMetallic)
		{
			frigateMetallic->CreateShaderResourceView(frigateMetallic->GetResource()->GetDesc().Format,
			                                          D3D12_SRV_DIMENSION_TEXTURE2D);
		}


		auto frigateRoughness = resourceManager.CreateTextureFromFile(
			"assets/textures/T_Frigate_BE2/T_Frigate_BE2_Roughness.tga", "Frigate_Roughness").lock();

		if (frigateRoughness)
		{
			frigateRoughness->CreateShaderResourceView(frigateRoughness->GetResource()->GetDesc().Format,
			                                           D3D12_SRV_DIMENSION_TEXTURE2D);
		}


		auto frigateNormal = resourceManager.CreateTextureFromFile(
			"assets/textures/T_Frigate_BE2/T_Frigate_BE2_Norm.tga", "Frigate_Normal").lock();

		if (frigateNormal)
		{
			frigateNormal->CreateShaderResourceView(frigateNormal->GetResource()->GetDesc().Format,
			                                        D3D12_SRV_DIMENSION_TEXTURE2D);
		}


		//=== Asteroid Textures =================================
		auto asteroidBlueAlbedo = resourceManager.CreateTextureFromFile(
			"assets/textures/Asteroid/AstOre_1_A.tga", "Asteroid_Blue_Albedo").lock();

		if (asteroidBlueAlbedo)
		{
			asteroidBlueAlbedo->CreateShaderResourceView(asteroidBlueAlbedo->GetResource()->GetDesc().Format,
			                                             D3D12_SRV_DIMENSION_TEXTURE2D);
		}


		auto asteroidBlueMetallic = resourceManager.CreateTextureFromFile(
			"assets/textures/Asteroid/AstOre_1_M.tga", "Asteroid_Blue_Metallic").lock();
		if (asteroidBlueMetallic)
		{
			asteroidBlueMetallic->CreateShaderResourceView(asteroidBlueMetallic->GetResource()->GetDesc().Format,
			                                               D3D12_SRV_DIMENSION_TEXTURE2D);
		}


		auto asteroidBlueRoughness = resourceManager.CreateTextureFromFile(
			"assets/textures/Asteroid/AstOre_1_R.tga", "Asteroid_Blue_Roughness").lock();
		if (asteroidBlueRoughness)
		{
			asteroidBlueRoughness->CreateShaderResourceView(asteroidBlueRoughness->GetResource()->GetDesc().Format,
			                                                D3D12_SRV_DIMENSION_TEXTURE2D);
		}


		auto asteroidBlueNormal = resourceManager.CreateTextureFromFile(
			"assets/textures/Asteroid/AstOre_1_N.tga", "Asteroid_Blue_Normal").lock();
		if (asteroidBlueNormal)
		{
			asteroidBlueNormal->CreateShaderResourceView(asteroidBlueNormal->GetResource()->GetDesc().Format,
			                                             D3D12_SRV_DIMENSION_TEXTURE2D);
		}


		auto asteroidBlueEmissive = resourceManager.CreateTextureFromFile(
			"assets/textures/Asteroid/AstOre_1_E.tga", "Asteroid_Blue_Emissive").lock();
		if (asteroidBlueEmissive)
		{
			asteroidBlueEmissive->CreateShaderResourceView(asteroidBlueEmissive->GetResource()->GetDesc().Format,
			                                               D3D12_SRV_DIMENSION_TEXTURE2D);
		}


		//=== Kraken Textures =================================

		//=== body ========
		auto krakenBodyAlbedo = resourceManager.CreateTextureFromFile(
			"assets/textures/Kraken/Tex_KRAKEN_BODY_BaseColor.tga", "Kraken_Body_Albedo").lock();
		if (krakenBodyAlbedo)
		{
			krakenBodyAlbedo->CreateShaderResourceView(krakenBodyAlbedo->GetResource()->GetDesc().Format,
			                                           D3D12_SRV_DIMENSION_TEXTURE2D);
		}


		auto krakenBodyRoughness = resourceManager.CreateTextureFromFile(
			"assets/textures/Kraken/T_M_KRAKEN_Mat_KRAKEN_MAIN_BODY_Roughness.tga", "Kraken_Body_Roughness").lock();
		if (krakenBodyRoughness)
		{
			krakenBodyRoughness->CreateShaderResourceView(krakenBodyRoughness->GetResource()->GetDesc().Format,
			                                              D3D12_SRV_DIMENSION_TEXTURE2D);
		}


		auto krakenBodyNormal = resourceManager.CreateTextureFromFile(
			"assets/textures/Kraken/Tex_KRAKEN_BODY_NRM.tga", "Kraken_Body_Normal").lock();
		if (krakenBodyNormal)
		{
			krakenBodyNormal->CreateShaderResourceView(krakenBodyNormal->GetResource()->GetDesc().Format,
			                                           D3D12_SRV_DIMENSION_TEXTURE2D);
		}


		//=== tentacle =====
		auto krakenTentacleAlbedo = resourceManager.CreateTextureFromFile(
			"assets/textures/Kraken/Tex_KRAKEN_LEG_TENTACLE_BaseColor.tga", "Kraken_Tentacle_Albedo").lock();
		if (krakenTentacleAlbedo)
		{
			krakenTentacleAlbedo->CreateShaderResourceView(krakenTentacleAlbedo->GetResource()->GetDesc().Format,
			                                               D3D12_SRV_DIMENSION_TEXTURE2D);
		}


		auto krakenTentacleRoughness = resourceManager.CreateTextureFromFile(
			"assets/textures/Kraken/T_M_KRAKEN_Mat_TENTACLES_LEGS_CLAWS_Roughness.tga",
			"Kraken_Tentacle_Roughness").lock();
		if (krakenTentacleRoughness)
		{
			krakenTentacleRoughness->CreateShaderResourceView(krakenTentacleRoughness->GetResource()->GetDesc().Format,
			                                                  D3D12_SRV_DIMENSION_TEXTURE2D);
		}


		auto krakenTentacleNormal = resourceManager.CreateTextureFromFile(
			"assets/textures/Kraken/Tex_KRAKEN_LEG_TENTACLE_CLAW_NRM.tga", "Kraken_Tentacle_Normal").lock();
		if (krakenTentacleNormal)
		{
			krakenTentacleNormal->CreateShaderResourceView(krakenTentacleNormal->GetResource()->GetDesc().Format,
			                                               D3D12_SRV_DIMENSION_TEXTURE2D);
		}

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


		
		auto sun2 = m_World->SpawnActor<Sun>({ "Sun" }).lock();
		sun2->SetPosition({ +100000.0f, -200000.0f, +200000.0f });
		auto sunMesh2 = Crystal::Cast<Crystal::StaticMeshComponent>(sun2->GetComponentByClass("StaticMeshComponent"));
		auto lightComponent = Crystal::Cast<Crystal::LightComponent>(sun2->GetComponentByClass("LocalLightComponent"));
		lightComponent->SetLightColor({243.0f / 255.0f, 138.0f / 255.0f, 110.0f / 255.0f });
		lightComponent->SetLightIntensity(3.0f);
		

		sunMesh2->SetRenderable(resourceManager.GetRenderable("Sphere"));
		auto sunMat2 = sunMesh2->GetMaterial(0);
		sunMat2->EmissiveColor = { 243.0f / 255.0f * 3.0f, 138.0f / 255.0f * 3.0f, 110.0f / 255.0f * 3.0f };

		


		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
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


		/*auto particleActor = m_World->SpawnActor<Crystal::ParticleActor>({}).lock();

		auto particleComponent = Crystal::Cast<Crystal::ParticleComponent>(
			particleActor->GetComponentByClass("ParticleComponent"));
		particleComponent->SetRenderable(resourceManager.GetRenderable("3DPlaneQuadMesh"));
		auto& particleMats = particleComponent->GetMaterials();
		auto particleMat = particleMats[0].get();
		particleMat->EmissiveTexture = resourceManager.GetTexture("Asteroid_Blue_Albedo");*/


		auto playerStart = m_World->SpawnActor<Crystal::PlayerStartActor>({"1"}).lock();
		playerStart->SetPosition({0.0f, 0.0f, -2000.0f});
		auto playerStart2 = m_World->SpawnActor<Crystal::PlayerStartActor>({"2"}).lock();
		playerStart2->SetPosition({0.0f, 0.0f, -4000.0f});
		auto playerStart3 = m_World->SpawnActor<Crystal::PlayerStartActor>({"3"}).lock();
		playerStart3->SetPosition({0.0f, 0.0f, -6000.0f});
		auto playerStart4 = m_World->SpawnActor<Crystal::PlayerStartActor>({"4"}).lock();
		playerStart4->SetPosition({0.0f, 0.0f, -8000.0f});


		auto kraken = m_World->SpawnActor<Kraken>({}).lock();
		auto meshComponent = Crystal::Cast<Crystal::SkeletalMeshComponent>(
			kraken->GetComponentByClass("SkeletalMeshComponent"));
		meshComponent->SetRenderable(resourceManager.GetRenderable("Kraken"));
		
		auto bodyMaterial = meshComponent->GetMaterial(0);
		bodyMaterial->AlbedoTexture = resourceManager.GetTexture("Kraken_Body_Albedo");
		bodyMaterial->RoughnessTexture = resourceManager.GetTexture("Kraken_Body_Roughness");
		bodyMaterial->NormalTexture = resourceManager.GetTexture("Kraken_Body_Normal");
		
		auto tentacleMaterial = meshComponent->GetMaterial(1);
		tentacleMaterial->AlbedoTexture = resourceManager.GetTexture("Kraken_Tentacle_Albedo");
		tentacleMaterial->RoughnessTexture = resourceManager.GetTexture("Kraken_Tentacle_Roughness");
		tentacleMaterial->NormalTexture = resourceManager.GetTexture("Kraken_Tentacle_Normal");




		/*auto newActorMat = newActorMesh->GetMaterial(0);
		newActorMat->AlbedoTexture = resourceManager.GetTexture("Frigate_Albedo");
		newActorMat->MetallicTexture = resourceManager.GetTexture("Frigate_Metallic");
		newActorMat->RoughnessTexture = resourceManager.GetTexture("Frigate_Roughness");
		newActorMat->NormalTexture = resourceManager.GetTexture("Frigate_Normal");*/


#endif

#ifdef CS_NM_CLIENT

#endif


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
