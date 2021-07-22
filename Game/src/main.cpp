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
#include "actors/Missile.h"
#include "actors/MyHUD.h"
#include "actors/PlayCircle.h"
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
			auto sun = m_World->SpawnActor<Sun>({"Sun"}).lock();
			sun->SetPosition({0.0f, 20000.0f, 20000.0f});
			

			auto sunMesh = Crystal::Cast<Crystal::StaticMeshComponent>(sun->GetComponentByClass("StaticMeshComponent"));
			auto sunMat = sunMesh->GetMaterial(0);
			sunMat->EmissiveColor = DirectX::XMFLOAT3(1.0f * 3.0f, 1.0f * 3.0f, 0.4f * 3.0f);
		}

		{
			auto sun2 = m_World->SpawnActor<Sun>({"Sun"}).lock();
			sun2->SetPosition({+10000.0f, -20000.0f, +20000.0f});
			
			
			
			auto lightComponent = Crystal::Cast<Crystal::LightComponent>(sun2->GetComponentByClass("DirectionalLightComponent"));
			lightComponent->SetLightColor({ 243.0f / 255.0f, 138.0f / 255.0f, 110.0f / 255.0f });
			lightComponent->SetLightIntensity(3.0f);
			lightComponent->RotatePitch(90.0f);
			lightComponent->SetCastShadow(true);

			auto sunMesh2 = Crystal::Cast<Crystal::StaticMeshComponent>(sun2->GetComponentByClass("StaticMeshComponent"));
			auto sunMat2 = sunMesh2->GetMaterial(0);
			sunMat2->EmissiveColor = {243.0f / 255.0f * 3.0f, 138.0f / 255.0f * 3.0f, 110.0f / 255.0f * 3.0f};
		}

		if (true)
		{
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					for (int k = 0; k < 10; k++)
					{
						//test
						int randomNumber = rand() % 3;
						randomNumber = 3;
						switch (randomNumber)
						{
						case 0:
							{
								auto asteroid = m_World->SpawnActor<HealAsteroid>({}).lock();
								asteroid->SetPosition({100.0f * i, 100.0f * k, 100.0f * j});
								break;
							}
						case 1:
							{
								auto asteroid = m_World->SpawnActor<PowerAsteroid>({}).lock();
								asteroid->SetPosition({100.0f * i, 100.0f * k, 100.0f * j});
								break;
							}
						case 2:
							{
								auto asteroid = m_World->SpawnActor<ShieldAsteroid>({}).lock();
								asteroid->SetPosition({100.0f * i, 100.0f * k, 100.0f * j});
								break;
							}
						case 3:
							{
								auto asteroid = m_World->SpawnActor<Asteroid>({}).lock();
								asteroid->SetPosition(Crystal::Vector3::RandomPositionInSphere(Crystal::Vector3::Zero, 10000.0f));
								break;
							}
						}
					}
				}
			}
		}

		if (false)
		{
			for (int i = 0; i < 2; i++)
			{
				auto kraken = m_World->SpawnActor<Kraken>({}).lock();
				kraken->SetPosition({0.0f + 1000.0f * i, 0.0f, 0.0f});
			}
		}


		{
			auto playerStart = m_World->SpawnActor<Crystal::PlayerStartActor>({"1"}).lock();
			playerStart->SetPosition({0.0f, 0.0f, 0.0f});
			auto playerStart2 = m_World->SpawnActor<Crystal::PlayerStartActor>({"2"}).lock();
			playerStart2->SetPosition({0.0f, 0.0f, -4000.0f});
			auto playerStart3 = m_World->SpawnActor<Crystal::PlayerStartActor>({"3"}).lock();
			playerStart3->SetPosition({0.0f, 0.0f, -6000.0f});
			auto playerStart4 = m_World->SpawnActor<Crystal::PlayerStartActor>({"4"}).lock();
			playerStart4->SetPosition({0.0f, 0.0f, -8000.0f});
		}


		m_World->GetCurrentLevel()->OnClientConnect();


		if (true)
		{
			auto particleActor = m_World->SpawnActor<Crystal::ParticleActor>({""}).lock();
		}

		if (false)
		{
			auto staticMeshActor = m_World->SpawnActor<Crystal::StaticMeshActor>({""}).lock();
			staticMeshActor->SetPosition({0.0f, -1000.0f, 0.0f});
			staticMeshActor->SetUnitScale(50.0f);


			auto staticMeshComponent = Crystal::Cast<Crystal::StaticMeshComponent>(
				staticMeshActor->GetComponentByName("StaticMeshComponent"));
			staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/floor.fbx"));

			staticMeshComponent->RotatePitch(90.0f);

			auto& materials = staticMeshComponent->GetMaterials();
			auto pbrMat = materials[0].get();
			pbrMat->AlbedoColor = Crystal::Vector3::White;
		}


		auto playCircle = m_World->SpawnActor<PlayCircle>({""}).lock();
		auto playCircle2 = m_World->SpawnActor<PlayCircle>({""}).lock();
		playCircle2->SetPosition({ 0.0f, 0.0f, -1000.0f });
		auto staticMesh = Crystal::Cast<Crystal::StaticMeshComponent>(playCircle2->GetComponentByClass("StaticMeshComponent"));
		staticMesh->GetMaterials()[0]->EmissiveColor = Crystal::Vector3::Blue;
		


#endif

#ifdef CS_NM_CLIENT

#endif
	}
};

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080


Register_Application(GameApplication, WINDOW_WIDTH, WINDOW_HEIGHT);
