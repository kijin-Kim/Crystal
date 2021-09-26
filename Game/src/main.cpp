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

#include "actors/AttackLevel.h"
#include "actors/CustomBTTask.h"
#include "actors/Drone.h"
#include "actors/DroneAIController.h"
#include "actors/EndLevels.h"
#include "actors/EscortLevel.h"
#include "actors/MainLevel.h"
#include "actors/KrakenLevel.h"
#include "actors/Kraken.h"
#include "actors/Missile.h"
#include "actors/KrakenAIController.h"
#include "actors/LevelClearedLevel.h"
#include "actors/MiningLevel.h"
#include "actors/MyHUD.h"
#include "actors/ShieldSphere.h"
#include "actors/ShopLevel.h"
#include "actors/SpaceWhale.h"
#include "actors/StargateLevel.h"
#include "actors/TitleLevel.h"
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

		m_World->CreateLevel<TitleLevel>("TitleLevel");
		m_World->CreateLevel<EscortLevel>("EscortLevel");
		m_World->CreateLevel<MiningLevel>("MiningLevel");
		m_World->CreateLevel<KrakenLevel>("KrakenLevel");
		m_World->CreateLevel<StargateLevel>("StargateLevel");
		m_World->CreateLevel<ShopLevel>("ShopLevel");
		m_World->CreateLevel<LevelClearedLevel>("LevelClearedLevel");
		m_World->CreateLevel<MainLevel>("MainLevel");
		m_World->CreateLevel<AttackLevel>("AttackLevel");
		m_World->PushLevel("TitleLevel");

	}
};

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080


Register_Application(GameApplication, WINDOW_WIDTH, WINDOW_HEIGHT);
