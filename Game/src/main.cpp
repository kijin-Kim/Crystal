#include <Crystal.h>
#include <Crystal/Core/EntryPoint.h>

#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/GamePlay/Objects/Actors/CameraPawn.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"

#include "actors/MyPlayerPawn.h"
#include "actors/Asteroid.h"
#include "Crystal/Resources/ResourceManager.h"
#include "actors/Sun.h"
#include "Crystal/GamePlay/Objects/Actors/GameMode.h"
#include "Crystal/GamePlay/Objects/Actors/ParticleActor.h"
#include "Crystal/GamePlay/Objects/Actors/PlayerStartActor.h"

#include <sstream>

#include "actors/CustomBTTask.h"
#include "actors/Drone.h"
#include "actors/DroneAIController.h"
#include "actors/Kraken.h"
#include "actors/Missile.h"
#include "actors/KrakenAIController.h"
#include "actors/MyHUD.h"
#include "actors/ShieldSphere.h"
#include "actors/SpaceWhale.h"
#include "Crystal/GamePlay/AI/BehaviorTree.h"
#include "Crystal/GamePlay/AI/Blackboard.h"
#include "Crystal/GamePlay/AI/Decorator.h"
#include "Crystal/GamePlay/Objects/Actors/BoundingOrientedBoxActor.h"
#include "Crystal/GamePlay/Objects/Actors/PostProcessVolumeActor.h"
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


		auto& resourceManager = Crystal::ResourceManager::Instance();

		{
			resourceManager.GetTexture("assets/textures/Whale Diffuse.jpg");
			resourceManager.GetTexture("assets/textures/Whale Roughness.jpg");
			resourceManager.GetTexture("assets/textures/Whale Normals.jpg");
			resourceManager.GetTexture("assets/textures/Whale Emissive.jpg");

			resourceManager.GetRenderable<Crystal::SkeletalMesh>("assets/models/Biomechanical Whale Animated.fbx");
			resourceManager.GetAnimation("assets/models/Biomechanical Whale Animated.fbx");


			resourceManager.GetTexture("assets/textures/Kraken/Tex_KRAKEN_BODY_BaseColor.tga");
			resourceManager.GetTexture("assets/textures/Kraken/T_M_KRAKEN_Mat_KRAKEN_MAIN_BODY_Roughness.tga");
			resourceManager.GetTexture("assets/textures/Kraken/Tex_KRAKEN_BODY_NRM.tga");

			resourceManager.GetTexture("assets/textures/Kraken/Tex_KRAKEN_LEG_TENTACLE_BaseColor.tga");
			resourceManager.GetTexture("assets/textures/Kraken/T_M_KRAKEN_Mat_TENTACLES_LEGS_CLAWS_Roughness.tga");
			resourceManager.GetTexture("assets/textures/Kraken/Tex_KRAKEN_LEG_TENTACLE_CLAW_NRM.tga");

			resourceManager.GetRenderable<Crystal::SkeletalMesh>("assets/models/KRAKEN.fbx");


			resourceManager.GetAnimation("assets/models/KRAKEN_smashAttack.fbx");
			resourceManager.GetAnimation("assets/models/KRAKEN_idle.fbx");

		}


		{
			auto playerStart = m_World->SpawnActor<Crystal::PlayerStartActor>({"1"}).lock();
			playerStart->SetPosition({0.0f, 1000.0f, 0.0f});
		}

		m_World->GetCurrentLevel()->OnClientConnect();

		if(true)
		{
			for(int i=0; i< 0; i++)
			{
				auto drone = m_World->SpawnActor<Drone>({ "Drone" }).lock();
				drone->SetPosition(Crystal::Vector3::RandomPositionInSphere(Crystal::Vector3::Zero, 3000.0f));

				auto droneController = m_World->SpawnActor<DroneAIController>({}).lock();
				droneController->Possess(drone);
				
			}
			
		}


		{
			auto sun = m_World->SpawnActor<Sun>({"Sun"}).lock();
			sun->SetPosition({+10000.0f, 20000.0f, 0.0f});

			auto lightComponent = Crystal::Cast<Crystal::LightComponent>(sun->GetComponentByClass("DirectionalLightComponent"));
			lightComponent->SetLightColor({1.0f, 1.0f , 1.0f });
			
			lightComponent->SetLightIntensity(7.0f);
			lightComponent->RotateYaw(-90.0f);
			lightComponent->RotatePitch(45.0f);
			lightComponent->SetCastShadow(true);

			auto sunMesh = Crystal::Cast<Crystal::StaticMeshComponent>(sun->GetComponentByClass("StaticMeshComponent"));
			auto sunMat = sunMesh->GetMaterial(0);
			sunMat->EmissiveColor = { 1.0f  * 7.0f, 1.0f * 7.0f, 90.0f / 255.0f * 7.0f};
		}



		{
			auto lightActor = m_World->SpawnActor<Crystal::DirectionalLightActor>({ "LightActor2" }).lock();
			auto lightComponent = Crystal::Cast<Crystal::LightComponent>(lightActor->GetComponentByClass("DirectionalLightComponent"));
			lightComponent->SetLightColor({ 1.0f, 1.0f, 1.0f });
			lightComponent->SetLightIntensity(2.0f);
			lightComponent->RotateYaw(135.0f);
			lightComponent->RotatePitch(30.0f);
			lightComponent->SetCastShadow(false);
		}

		{
			const float distFromCenter = 15000.0f;
			const float boundingOrientedBoxWidth = 500.0f;

			auto boundingOrientedBoxActor1 = m_World->SpawnActor<Crystal::BoundingOrientedBoxActor>({ "BoundingOrientedBoxActor1" }).lock();
			boundingOrientedBoxActor1->SetPosition({ +distFromCenter + boundingOrientedBoxWidth, 0.0f, 0.0f });
			boundingOrientedBoxActor1->SetExtents({ boundingOrientedBoxWidth, +distFromCenter + boundingOrientedBoxWidth * 2.0f, +distFromCenter + boundingOrientedBoxWidth * 2.0f });
			
			auto boundingOrientedBoxActor2 = m_World->SpawnActor<Crystal::BoundingOrientedBoxActor>({ "BoundingOrientedBoxActor2" }).lock();
			boundingOrientedBoxActor2->SetPosition({ -distFromCenter - boundingOrientedBoxWidth, 0.0f, 0.0f });
			boundingOrientedBoxActor2->SetExtents({ boundingOrientedBoxWidth, +distFromCenter + boundingOrientedBoxWidth * 2.0f, +distFromCenter + boundingOrientedBoxWidth * 2.0f });

			auto boundingOrientedBoxActor3 = m_World->SpawnActor<Crystal::BoundingOrientedBoxActor>({ "BoundingOrientedBoxActor3" }).lock();
			boundingOrientedBoxActor3->SetPosition({ 0.0f, +distFromCenter + boundingOrientedBoxWidth, 0.0f });
			boundingOrientedBoxActor3->SetExtents({ boundingOrientedBoxWidth, +distFromCenter + boundingOrientedBoxWidth * 2.0f, +distFromCenter + boundingOrientedBoxWidth * 2.0f });
			boundingOrientedBoxActor3->RotateRoll(90.0f);
			auto boundingOrientedBoxActor4 = m_World->SpawnActor<Crystal::BoundingOrientedBoxActor>({ "BoundingOrientedBoxActor4" }).lock();
			boundingOrientedBoxActor4->SetPosition({ 0.0f, -distFromCenter - boundingOrientedBoxWidth, 0.0f });
			boundingOrientedBoxActor4->SetExtents({ boundingOrientedBoxWidth, +distFromCenter + boundingOrientedBoxWidth * 2.0f, +distFromCenter + boundingOrientedBoxWidth * 2.0f });
			boundingOrientedBoxActor4->RotateRoll(90.0f);

			auto boundingOrientedBoxActor5 = m_World->SpawnActor<Crystal::BoundingOrientedBoxActor>({ "BoundingOrientedBoxActor5" }).lock();
			boundingOrientedBoxActor5->SetPosition({ 0.0f, 0.0f, +distFromCenter + boundingOrientedBoxWidth });
			boundingOrientedBoxActor5->SetExtents({ boundingOrientedBoxWidth, +distFromCenter + boundingOrientedBoxWidth * 2.0f, +distFromCenter + boundingOrientedBoxWidth * 2.0f });
			boundingOrientedBoxActor5->RotateYaw(90.0f);

			auto boundingOrientedBoxActor6 = m_World->SpawnActor<Crystal::BoundingOrientedBoxActor>({ "BoundingOrientedBoxActor6" }).lock();
			boundingOrientedBoxActor6->SetPosition({ 0.0f, 0.0f, -distFromCenter - boundingOrientedBoxWidth });
			boundingOrientedBoxActor6->SetExtents({ boundingOrientedBoxWidth, +distFromCenter + boundingOrientedBoxWidth * 2.0f, +distFromCenter + boundingOrientedBoxWidth * 2.0f });
			boundingOrientedBoxActor6->RotateYaw(90.0f);
		}

	

		if (true)
		{
			for (int i = 0; i < 0; i++)
			{
				auto asteroid = m_World->SpawnActor<Asteroid>({}).lock();
				asteroid->SetPosition(Crystal::Vector3::RandomPositionInSphere(Crystal::Vector3::Zero, 15000.0f));
			}

			for (int i = 0; i < 0; i++)
			{
				auto asteroid = m_World->SpawnActor<FakeAsteroid>({}).lock();
				asteroid->SetPosition(Crystal::Vector3::RandomPositionInSphere(Crystal::Vector3::Zero, 10000.0f));
			}


			for (int i = 0; i < 30; i++)
			{
				int randomNumber = rand() % 3;
				switch (randomNumber)
				{
				case 0:
					{
						auto asteroid = m_World->SpawnActor<HealAsteroid>({}).lock();
						asteroid->SetPosition(Crystal::Vector3::RandomPositionInSphere(Crystal::Vector3::Zero, 5000.0f));
						break;
					}
				case 1:
					{
						auto asteroid = m_World->SpawnActor<PowerAsteroid>({}).lock();
						asteroid->SetPosition(Crystal::Vector3::RandomPositionInSphere(Crystal::Vector3::Zero, 5000.0f));
						break;
					}
				case 2:
					{
						auto asteroid = m_World->SpawnActor<ShieldAsteroid>({}).lock();
						asteroid->SetPosition(Crystal::Vector3::RandomPositionInSphere(Crystal::Vector3::Zero, 5000.0f));
						break;
					}
				}
			}
		}


		if (true)
		{
			auto kraken = m_World->SpawnActor<Kraken>({}).lock();
			kraken->SetPosition({0.0f, 0.0f, 0.0f});
			auto krakenController = m_World->SpawnActor<KrakenAIController>({}).lock();
			krakenController->Possess(kraken);
		}


		if (false)
		{
			auto particleActor = m_World->SpawnActor<Crystal::ParticleActor>({""}).lock();
		}


		if (false)
		{
			auto playCircle = m_World->SpawnActor<ShieldSphere>({""}).lock();
		}

		if (true)
		{
			auto postProcessActor = m_World->SpawnActor<Crystal::PostProcessVolumeActor>({ "GreenTintPostProcess" }).lock();
			postProcessActor->SetPostProcessOrder(0);
			postProcessActor->SetVolumeBehavior(Crystal::EVolumeBehavior::VB_EnableWhenNotOverlap);
			postProcessActor->SetVolumeType(Crystal::EVolumeType::VT_Sphere);
			postProcessActor->SetSphereRadius(2300.0f);
			postProcessActor->SetPosition(Crystal::Vector3::Zero);
			postProcessActor->SetIsEnabled(false);
			
			auto postProcessComponent = postProcessActor->GetPostProcessComponent();

			auto mat = postProcessComponent->GetMaterials()[0];
			mat->TintColor = { 0.2f, 0.2f, 0.05f };

			auto kraken = Crystal::Cast<Kraken>(m_World->GetCurrentLevel()->GetActorByClass("Kraken"));
			kraken->GreenTintVolumeActor = postProcessActor;
		}

		if (true)
		{
			auto postProcessActor = m_World->SpawnActor<Crystal::PostProcessVolumeActor>({ "VignettePostProcess"}).lock();
			postProcessActor->SetPostProcessOrder(2);
			postProcessActor->SetVolumeBehavior(Crystal::EVolumeBehavior::VB_EnableWhenNotOverlap);
			postProcessActor->SetVolumeType(Crystal::EVolumeType::VT_Sphere);
			postProcessActor->SetSphereRadius(2300.0f);
			postProcessActor->SetIsEnabled(false);

			auto postProcessComponent = postProcessActor->GetPostProcessComponent();

			auto mat = postProcessComponent->GetMaterials()[0];
			mat->SceneColorMaskTexture = resourceManager.GetTexture("assets/textures/radial_gradient.png");

			auto kraken = Crystal::Cast<Kraken>(m_World->GetCurrentLevel()->GetActorByClass("Kraken"));
			kraken->VignetteVolumeActor = postProcessActor;
		}

		if(true)
		{
			auto postProcessActor = m_World->SpawnActor<Crystal::PostProcessVolumeActor>({ "CharacterDamagedPostProcess" }).lock();
			postProcessActor->SetPostProcessOrder(3);
			postProcessActor->SetUnBound(true);


			auto postProcessComponent = postProcessActor->GetPostProcessComponent();

			auto mat = postProcessComponent->GetMaterials()[0];
			mat->AlbedoColor = Crystal::Vector3::Red;
			mat->OpacityTexture = resourceManager.GetTexture("assets/textures/radial_gradient2_inv.png");
			mat->OpacityMultiplier = 0.0f;

			auto myPlayerPawn = Crystal::Cast<MyPlayerPawn>(m_World->GetCurrentLevel()->GetActorByClass("MyPlayerPawn"));
			myPlayerPawn->DamagedPostProcessActor = postProcessActor;
		}

		if (true)
		{
			auto postProcessActor = m_World->SpawnActor<Crystal::PostProcessVolumeActor>({ "CharacterHealPostProcess" }).lock();
			postProcessActor->SetPostProcessOrder(3);
			postProcessActor->SetUnBound(true);


			auto postProcessComponent = postProcessActor->GetPostProcessComponent();

			auto mat = postProcessComponent->GetMaterials()[0];
			mat->AlbedoColor = { 51.0f / 255.0f, 165.0f / 255.0f, 50.0f / 255.0f };
			mat->OpacityTexture = resourceManager.GetTexture("assets/textures/radial_gradient_inv.png");
			mat->OpacityMultiplier = 0.0f;

			auto myPlayerPawn = Crystal::Cast<MyPlayerPawn>(m_World->GetCurrentLevel()->GetActorByClass("MyPlayerPawn"));
			myPlayerPawn->HealPostProcessActor = postProcessActor;
		}

		if (true)
		{
			auto postProcessActor = m_World->SpawnActor<Crystal::PostProcessVolumeActor>({ "CharacterShieldPostProcess" }).lock();
			postProcessActor->SetPostProcessOrder(1);
			postProcessActor->SetUnBound(true);
			postProcessActor->SetHiddenInGame(true);

			auto postProcessComponent = postProcessActor->GetPostProcessComponent();

			auto mat = postProcessComponent->GetMaterials()[0];
			mat->AlbedoColor = { 21.0f / 255.0f, 105.0f / 255.0f, 199.0f / 255.0f };
			mat->OpacityTexture = resourceManager.GetTexture("assets/textures/bump_reverse.png");
			mat->TexCoordMultiplier = { 3.0f, 3.0f * (1080.0f / 1920.0f) };
			mat->OpacityMultiplier = 0.3f;
			

			auto myPlayerPawn = Crystal::Cast<MyPlayerPawn>(m_World->GetCurrentLevel()->GetActorByClass("MyPlayerPawn"));
			myPlayerPawn->ShieldPostProcess = postProcessActor;
		}

		
	}
};

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080


Register_Application(GameApplication, WINDOW_WIDTH, WINDOW_HEIGHT);
