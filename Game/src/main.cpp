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
#include "Crystal/GamePlay/Objects/Actors/StaticMeshActor.h"


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


		{
			/*auto sun = m_World->SpawnActor<Sun>({"Sun"}).lock();
			sun->SetPosition({0.0f, 200000.0f, 200000.0f});
			auto sunMesh = Crystal::Cast<Crystal::StaticMeshComponent>(sun->GetComponentByClass("StaticMeshComponent"));

			sunMesh->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/Sphere.fbx"));
			auto sunMat = sunMesh->GetMaterial(0);
			sunMat->EmissiveColor = DirectX::XMFLOAT3(1.0f * 3.0f, 1.0f * 3.0f, 0.4f * 3.0f);*/
		}

		{
			auto sun2 = m_World->SpawnActor<Sun>({"Sun"}).lock();
			sun2->SetPosition({+100000.0f, -200000.0f, +200000.0f});
			auto sunMesh2 = Crystal::Cast<Crystal::StaticMeshComponent>(
				sun2->GetComponentByClass("StaticMeshComponent"));
			auto lightComponent = Crystal::Cast<Crystal::LightComponent>(
				sun2->GetComponentByClass("DirectionalLightComponent"));
			lightComponent->SetLightColor({243.0f / 255.0f, 138.0f /
				255.0f, 110.0f / 255.0f});
			lightComponent->SetLightIntensity(3.0f);
			lightComponent->RotatePitch(90.0f);
			sunMesh2->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/Sphere.fbx"));
			auto sunMat2 = sunMesh2->GetMaterial(0);
			sunMat2->EmissiveColor = {243.0f / 255.0f * 3.0f, 138.0f / 255.0f * 3.0f, 110.0f / 255.0f * 3.0f};
		}

		if(true)
		{
			for (int i = 0; i < 5; i++)
			{
				for (int j = 0; j < 5; j++)
				{
					for (int k = 0; k < 1; k++)
					{
						//test

						auto asteroid = m_World->SpawnActor<Asteroid>({}).lock();
						asteroid->SetPosition({1000.0f * i, 1000.0f * j, 1000.0f * k});

						auto staticMeshComponent = Crystal::Cast<Crystal::StaticMeshComponent>(
							asteroid->GetComponentByName("StaticMeshComponent"));
						staticMeshComponent->SetRenderable(
							resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/Asteroid_1.fbx"));
						auto& materials = staticMeshComponent->GetMaterials();
						auto pbrMat = materials[0].get();
						pbrMat->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_1_A.tga");
						pbrMat->MetallicTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_1_M.tga");
						pbrMat->RoughnessTexture = resourceManager.
							GetTexture("assets/textures/Asteroid/AstOre_1_R.tga");
						pbrMat->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_1_N.tga");
						pbrMat->EmissiveTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_1_E.tga");
					}
				}
			}
		}

		if(false)
		{
			for(int i=0; i<2; i++)
			{
				auto kraken = m_World->SpawnActor<Kraken>({}).lock();
				kraken->SetPosition({ 0.0f + 1000.0f * i, 0.0f, 0.0f });
				
				auto meshComponent = Crystal::Cast<Crystal::SkeletalMeshComponent>(
					kraken->GetComponentByClass("SkeletalMeshComponent"));
				meshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::SkeletalMesh>(
					"assets/models/KRAKEN.fbx",
					"assets/models/KRAKEN_idle.fbx"));

				auto bodyMaterial = meshComponent->GetMaterial(0);
				bodyMaterial->AlbedoTexture = resourceManager.GetTexture(
					"assets/textures/Kraken/Tex_KRAKEN_BODY_BaseColor.tga");
				bodyMaterial->RoughnessTexture = resourceManager.GetTexture(
					"assets/textures/Kraken/T_M_KRAKEN_Mat_KRAKEN_MAIN_BODY_Roughness.tga");
				bodyMaterial->NormalTexture = resourceManager.GetTexture("assets/textures/Kraken/Tex_KRAKEN_BODY_NRM.tga");

				auto tentacleMaterial = meshComponent->GetMaterial(1);
				tentacleMaterial->AlbedoTexture = resourceManager.GetTexture(
					"assets/textures/Kraken/Tex_KRAKEN_LEG_TENTACLE_BaseColor.tga");
				tentacleMaterial->RoughnessTexture = resourceManager.GetTexture(
					"assets/textures/Kraken/T_M_KRAKEN_Mat_TENTACLES_LEGS_CLAWS_Roughness.tga");
				tentacleMaterial->NormalTexture = resourceManager.GetTexture(
					"assets/textures/Kraken/Tex_KRAKEN_LEG_TENTACLE_CLAW_NRM.tga");
			}
			
		}


		{
			auto playerStart = m_World->SpawnActor<Crystal::PlayerStartActor>({"1"}).lock();
			playerStart->SetPosition({0.0f, 0.0f, -2000.0f});
			auto playerStart2 = m_World->SpawnActor<Crystal::PlayerStartActor>({"2"}).lock();
			playerStart2->SetPosition({0.0f, 0.0f, -4000.0f});
			auto playerStart3 = m_World->SpawnActor<Crystal::PlayerStartActor>({"3"}).lock();
			playerStart3->SetPosition({0.0f, 0.0f, -6000.0f});
			auto playerStart4 = m_World->SpawnActor<Crystal::PlayerStartActor>({"4"}).lock();
			playerStart4->SetPosition({0.0f, 0.0f, -8000.0f});
		}


		m_World->GetCurrentLevel()->OnClientConnect();


		auto particleActor = m_World->SpawnActor<Crystal::ParticleActor>({ "" }).lock();

		{
			auto staticMeshActor = m_World->SpawnActor<Crystal::StaticMeshActor>({ "" }).lock();
			staticMeshActor->SetPosition({ 0.0f, -1000.0f, 0.0f });
			staticMeshActor->SetScale(50.0f);

			
			auto staticMeshComponent = Crystal::Cast<Crystal::StaticMeshComponent>(
				staticMeshActor->GetComponentByName("StaticMeshComponent"));
			staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/floor.fbx"));

			staticMeshComponent->RotatePitch(90.0f);
			
			auto& materials = staticMeshComponent->GetMaterials();
			auto pbrMat = materials[0].get();
			pbrMat->AlbedoColor = Crystal::Vector3::White;
		}
		
		


#endif

#ifdef CS_NM_CLIENT

#endif
	}
};

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080


Register_Application(GameApplication, WINDOW_WIDTH, WINDOW_HEIGHT);
