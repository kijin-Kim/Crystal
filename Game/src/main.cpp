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
#include "actors/Kraken.h"
#include "actors/Missile.h"
#include "actors/KrakenAIController.h"
#include "actors/MyHUD.h"
#include "actors/PlayCircle.h"
#include "actors/SpaceWhale.h"
#include "Crystal/GamePlay/AI/BehaviorTree.h"
#include "Crystal/GamePlay/AI/Blackboard.h"
#include "Crystal/GamePlay/AI/Decorator.h"
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
			lightComponent->SetLightColor({243.0f / 255.0f, 138.0f / 255.0f, 110.0f / 255.0f});
			lightComponent->SetLightIntensity(3.0f);
			lightComponent->RotatePitch(90.0f);
			lightComponent->SetCastShadow(true);
			

			auto sunMesh2 = Crystal::Cast<Crystal::StaticMeshComponent>(sun2->GetComponentByClass("StaticMeshComponent"));
			auto sunMat2 = sunMesh2->GetMaterial(0);
			sunMat2->EmissiveColor = {243.0f / 255.0f * 3.0f, 138.0f / 255.0f * 3.0f, 110.0f / 255.0f * 3.0f};
		}


		if (true)
		{
			for (int i = 0; i < 0; i++)
			{
				auto asteroid = m_World->SpawnActor<Asteroid>({}).lock();
				asteroid->SetPosition(Crystal::Vector3::RandomPositionInSphere(Crystal::Vector3::Zero, 1000.0f));
			}

			for (int i = 0; i < 0; i++)
			{
				int randomNumber = rand() % 3;
				switch (randomNumber)
				{
				case 0:
					{
						auto asteroid = m_World->SpawnActor<HealAsteroid>({}).lock();
						asteroid->SetPosition(Crystal::Vector3::RandomPositionInSphere(Crystal::Vector3::Zero, 3000.0f));
						break;
					}
				case 1:
					{
						auto asteroid = m_World->SpawnActor<PowerAsteroid>({}).lock();
						asteroid->SetPosition(Crystal::Vector3::RandomPositionInSphere(Crystal::Vector3::Zero, 3000.0f));
						break;
					}
				case 2:
					{
						auto asteroid = m_World->SpawnActor<ShieldAsteroid>({}).lock();
						asteroid->SetPosition(Crystal::Vector3::RandomPositionInSphere(Crystal::Vector3::Zero, 3000.0f));
						break;
					}
				}
			}
		}

		
		if (false)
		{
			auto kraken = m_World->SpawnActor<Kraken>({}).lock();
			kraken->SetPosition({ 0.0f, 0.0f, 0.0f });
			auto krakenController = m_World->SpawnActor<KrakenAIController>({}).lock();
			krakenController->Possess(kraken);
		}

		if(true)
		{
			auto spaceWhale = m_World->SpawnActor<SpaceWhale>({}).lock();
			spaceWhale->SetPosition({ 2000.0f, 0.0f, 0.0f });
			auto spaceWhaleController = m_World->SpawnActor<SpaceWhaleAIController>({}).lock();
			spaceWhaleController->Possess(spaceWhale);
			
			
			auto behaviorTree = Crystal::CreateObject<Crystal::BehaviorTree>();
			
			auto rootNode = behaviorTree->GetRootNode();
			auto selectorNode = Crystal::CreateObject<Crystal::BTSelectorNode>("Selector");
			//rootNode->AddChildNode(selectorNode);
			
			
			//// MoveToLastSeen
			//{
			//	auto sequenceNode = Crystal::CreateObject<Crystal::BTSequenceNode>("SequenceMoveToLastSeen");
			//	auto blackboardBasedDecorator = Crystal::CreateObject<Crystal::BlackboardBasedDecorator>();
			//	blackboardBasedDecorator->BlackboardKey = "PlayerLocation";
			//	blackboardBasedDecorator->bIsSet = true;
			//	sequenceNode->AddDecorator(blackboardBasedDecorator);

			//	auto faceLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeFaceLocation>("TaskFaceLocation");
			//	faceLocationNode->TargetLocationKey = "PlayerLocation";
			//	faceLocationNode->TargetAngleTolerance = 10.0f;
			//	sequenceNode->AddChildNode(faceLocationNode);

			//	auto moveToLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeMoveToLocation>("TaskMoveToLocation");
			//	moveToLocationNode->TargetLocationKey = "PlayerLocation";
			//	moveToLocationNode->AcceptableRadius = 120.0f;
			//	moveToLocationNode->MaxAcceleration = 30000000.0f;
			//	sequenceNode->AddChildNode(moveToLocationNode);

			//	auto clearValueNode = Crystal::CreateObject<Crystal::BTTaskNodeClearBlackboardValue>("TaskClearBlackboardValue");
			//	clearValueNode->BlackboardKey = "PlayerLocation";
			//	sequenceNode->AddChildNode(clearValueNode);

			//	
			//	selectorNode->AddChildNode(sequenceNode);
			//}


			// MoveToLastSeen
			{
				auto sequenceNode = Crystal::CreateObject<Crystal::BTSelectorNode>("SequenceMoveToLastSeen");
				auto blackboardBasedDecorator = Crystal::CreateObject<Crystal::BlackboardBasedDecorator>();
				blackboardBasedDecorator->BlackboardKey = "PlayerLocation";
				blackboardBasedDecorator->bIsSet = true;
				sequenceNode->AddDecorator(blackboardBasedDecorator);

				auto faceLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeFaceLocation>("TaskFaceLocation");
				faceLocationNode->TargetLocationKey = "PlayerLocation";
				faceLocationNode->TargetAngleTolerance = 10.0f;
				sequenceNode->AddChildNode(faceLocationNode);

				auto moveToLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeMoveToLocation>("TaskMoveToLocation");
				moveToLocationNode->TargetLocationKey = "PlayerLocation";
				moveToLocationNode->AcceptableRadius = 120.0f;
				moveToLocationNode->MaxAcceleration = 30000000.0f;
				sequenceNode->AddChildNode(moveToLocationNode);

				auto clearValueNode = Crystal::CreateObject<Crystal::BTTaskNodeClearBlackboardValue>("TaskClearBlackboardValue");
				clearValueNode->BlackboardKey = "PlayerLocation";
				sequenceNode->AddChildNode(clearValueNode);


				rootNode->AddChildNode(sequenceNode);
			}

			

			// Keep Orbit
			if(false)
			{
				auto sequenceNode = Crystal::CreateObject<Crystal::BTSequenceNode>("SequenceOrbit");

				auto setTargetDirectionNode = Crystal::CreateObject<BTTaskNodeSetTargetDirection>("TaskSetTargetDirection");
				setTargetDirectionNode->TargetDirectionKey = "TargetDirection";
				sequenceNode->AddChildNode(setTargetDirectionNode);

				auto moveTowardDirectionNode = Crystal::CreateObject<BTTaskNodeMoveTowardDirection>("TaskMoveTowardDirection");
				moveTowardDirectionNode->TargetDirectionKey = "TargetDirection";
				moveTowardDirectionNode->MaxAcceleration = 30000000.0f;
				sequenceNode->AddChildNode(moveTowardDirectionNode);
								
				selectorNode->AddChildNode(sequenceNode);				
			}
			
			

			
			spaceWhaleController->SetBehaviorTree(behaviorTree);
			
		}


		{
			auto playerStart = m_World->SpawnActor<Crystal::PlayerStartActor>({"1"}).lock();
			playerStart->SetPosition({0.0f, 0.0f, -1000.0f});
			auto playerStart2 = m_World->SpawnActor<Crystal::PlayerStartActor>({"2"}).lock();
			playerStart2->SetPosition({0.0f, 0.0f, -4000.0f});
			auto playerStart3 = m_World->SpawnActor<Crystal::PlayerStartActor>({"3"}).lock();
			playerStart3->SetPosition({0.0f, 0.0f, -6000.0f});
			auto playerStart4 = m_World->SpawnActor<Crystal::PlayerStartActor>({"4"}).lock();
			playerStart4->SetPosition({0.0f, 0.0f, -8000.0f});
		}


		m_World->GetCurrentLevel()->OnClientConnect();


		if (false)
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


		if(false)
		{
			auto playCircle = m_World->SpawnActor<PlayCircle>({ "" }).lock();
		}


		
		
		

#endif

#ifdef CS_NM_CLIENT

#endif
	}
};

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080


Register_Application(GameApplication, WINDOW_WIDTH, WINDOW_HEIGHT);
