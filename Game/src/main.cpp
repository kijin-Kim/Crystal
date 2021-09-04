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
#include "actors/EndLevels.h"
#include "actors/MainLevel.h"
#include "actors/GameLevel.h"
#include "actors/Kraken.h"
#include "actors/Missile.h"
#include "actors/KrakenAIController.h"
#include "actors/MiningLevel.h"
#include "actors/MyHUD.h"
#include "actors/ShieldSphere.h"
#include "actors/SpaceWhale.h"
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

		m_World->CreateLevel<MiningLevel>("MiningLevel");
		m_World->CreateLevel<MainLevel>("MainLevel");
		m_World->CreateLevel<GameLevel>("GameLevel");
		m_World->CreateLevel<GameOverLevel>("GameOverLevel");
		m_World->CreateLevel<GameClearLevel>("GameClearLevel");
		m_World->CreateLevel<TitleLevel>("TitleLevel");

		m_World->PushLevel("MainLevel");


	}
};

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080


Register_Application(GameApplication, WINDOW_WIDTH, WINDOW_HEIGHT);
