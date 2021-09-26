#pragma once

#pragma once
#include "AllyDrone.h"
#include "AllyDroneAIController.h"
#include "Asteroid.h"
#include "Cruiser.h"
#include "MineCave.h"
#include "MyHUD.h"
#include "Quest.h"
#include "Stargate.h"
#include "Sun.h"
#include "Crystal/GamePlay/Objects/Actors/PlayerStartActor.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Scene.h"
#include "Crystal/Resources/ResourceManager.h"


class EscortLevel : public Crystal::Level
{
public:
	EscortLevel() = default;
	~EscortLevel() override = default;


	void Initialize() override
	{
		Level::Initialize();

		auto& resourceManager = Crystal::ResourceManager::Instance();
		{
		}

		auto& scene = GetScene();


		scene->PanoramaCubeColorTexture = resourceManager.GetTexture("assets/textures/cubemaps/T_Skybox_14_HybrydNoise.hdr").lock();
		scene->CubemapColorTexture = Crystal::CreateShared<Crystal::Texture>(1024, 1024, 6, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                                                     D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		                                                                     D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
		                                                                     D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

		scene->PanoramaCubeAlphaTexture = resourceManager.GetTexture("assets/textures/cubemaps/T_Skybox_14_Alpha.hdr").lock();
		scene->CubemapAlphaTexture = Crystal::CreateShared<Crystal::Texture>(1024, 1024, 6, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                                                     D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		                                                                     D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
		                                                                     D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

		scene->PanoramaStarFarTexture = resourceManager.GetTexture("assets/textures/cubemaps/T_Stars_Far_Mixed.hdr").lock();
		scene->CubemapStarFarTexture = Crystal::CreateShared<Crystal::Texture>(1024, 1024, 6, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                                                       D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		                                                                       D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
		                                                                       D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);


		scene->PanoramaStarNearTexture = resourceManager.GetTexture("assets/textures/cubemaps/T_Stars_Near_Small.hdr").lock();
		scene->CubemapStarNearTexture = Crystal::CreateShared<Crystal::Texture>(1024, 1024, 6, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                                                        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		                                                                        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
		                                                                        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

		scene->SkyboxConfiguration.RedLevel = 0.25f;
		scene->SkyboxConfiguration.GreenLevel = 0.64f;
		scene->SkyboxConfiguration.BlueLevel = 0.97f;
		scene->SkyboxConfiguration.Power = 0.8f;
		scene->SkyboxConfiguration.Saturation = 1.1f;
		scene->SkyboxConfiguration.NearStarDesaturation = 0.0f;
		scene->SkyboxConfiguration.FarStarDesaturation = 0.0f;
		scene->SkyboxConfiguration.AdditiveColor = Crystal::Vector4::Zero;

		SpawnActor<Crystal::PlayerStartActor>({});
	}

	void OnLevelOpened(Crystal::Shared<Level> lastLevel) override
	{
		Level::OnLevelOpened(lastLevel);
		
		auto hud = Crystal::Cast<MyHUD>(m_HUD);
		hud->ShowCruiserHud();

		auto& resourceManager = Crystal::ResourceManager::Instance();

		


		m_Cruiser = SpawnActor<Cruiser>({}).lock();


		Crystal::Cast<MyHUD>(m_HUD)->SetQuestMainText(L"Escort Cruiser To Stargate");

		QuestReward reward;
		reward.Gold = 1500;
		reward.ItemType.push_back(EItemType::IT_HealPotion);
		reward.ItemType.push_back(EItemType::IT_PowerPotion);
		reward.ItemCount.push_back(2);
		reward.ItemCount.push_back(3);


		CreateQuest("Move Cruiser To Target Location", { 0.0f, 0.0f, 40000.0f }, m_Cruiser, reward);

		auto stargate = SpawnActor<Stargate>({}).lock();
		stargate->SetPosition({0.0f, 0.0f, 40000.0f});




		{
			auto sun = SpawnActor<Sun>({"Sun"}).lock();
			sun->SetPosition({+30000.0f, 50000.0f, 0.0f});

			auto lightComponent = Crystal::Cast<Crystal::LightComponent>(sun->GetComponentByClass("DirectionalLightComponent"));
			lightComponent->SetLightColor({1.0f, 1.0f, 1.0f});

			lightComponent->SetLightIntensity(7.0f);
			lightComponent->RotateYaw(-90.0f);
			lightComponent->RotatePitch(45.0f);
			lightComponent->SetCastShadow(true);

			auto sunMesh = Crystal::Cast<Crystal::StaticMeshComponent>(sun->GetComponentByClass("StaticMeshComponent"));
			auto sunMat = sunMesh->GetMaterial(0);
			sunMat->EmissiveColor = {1.0f * 7.0f, 1.0f * 7.0f, 90.0f / 255.0f * 7.0f};
		}


		{
			auto lightActor = SpawnActor<Crystal::DirectionalLightActor>({"LightActor2"}).lock();
			auto lightComponent = Crystal::Cast<Crystal::LightComponent>(lightActor->GetComponentByClass("DirectionalLightComponent"));
			lightComponent->SetLightColor({1.0f, 1.0f, 1.0f});
			lightComponent->SetLightIntensity(2.0f);
			lightComponent->RotateYaw(135.0f);
			lightComponent->RotatePitch(30.0f);
			lightComponent->SetCastShadow(false);
		}


		//auto asteroid = SpawnActor<WhiteAsteroid>({}).lock();
		//asteroid->SetAsteroidType(3);
		//asteroid->SetUnitScale(30.0f * 0.08f);
		//asteroid->SetPosition({ 0.0f, 15000.0f, 15000.0f });

		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 30; j++)
			{
				auto asteroid = SpawnActor<WhiteAsteroid>({}).lock();
				asteroid->SetPosition(Crystal::Vector3::RandomPositionInSphere({0.0f, 0.0f, 20000.0f + 40000.0f/ 5.0f * float(i)}, 20000.0f));
			}
		}

		for (int j = 0; j < 10; j++)
		{
			auto drone = SpawnActor<Drone>({ "Drone" }).lock();
			drone->SetPosition(Crystal::Vector3::RandomPositionInSphere({ 0.0f, 0.0f, 40000.0f }, 20000.0f));

			auto droneController = SpawnActor<DroneAIController>({}).lock();
			droneController->Possess(drone);
		}



		if (true)
		{
			auto postProcessActor = SpawnActor<Crystal::PostProcessVolumeActor>({ "CharacterDamagedPostProcess" }).lock();
			postProcessActor->SetPostProcessOrder(3);
			postProcessActor->SetUnBound(true);


			auto postProcessComponent = postProcessActor->GetPostProcessComponent();

			auto mat = postProcessComponent->GetMaterials()[0];
			mat->AlbedoColor = Crystal::Vector3::Red;
			mat->OpacityTexture = resourceManager.GetTexture("assets/textures/radial_gradient2_inv.png");
			mat->OpacityMultiplier = 0.0f;

			m_PlayerPawn = Crystal::Cast<MyPlayerPawn>(GetActorByClass("MyPlayerPawn"));
			m_PlayerPawn->DamagedPostProcessActor = postProcessActor;
		}

		if (true)
		{
			auto postProcessActor = SpawnActor<Crystal::PostProcessVolumeActor>({ "CharacterHealPostProcess" }).lock();
			postProcessActor->SetPostProcessOrder(3);
			postProcessActor->SetUnBound(true);


			auto postProcessComponent = postProcessActor->GetPostProcessComponent();

			auto mat = postProcessComponent->GetMaterials()[0];
			mat->AlbedoColor = { 51.0f / 255.0f, 165.0f / 255.0f, 50.0f / 255.0f };
			mat->OpacityTexture = resourceManager.GetTexture("assets/textures/radial_gradient_inv.png");
			mat->OpacityMultiplier = 0.0f;

			m_PlayerPawn->HealPostProcessActor = postProcessActor;
		}

		if (true)
		{
			auto postProcessActor = SpawnActor<Crystal::PostProcessVolumeActor>({ "CharacterShieldPostProcess" }).lock();
			postProcessActor->SetPostProcessOrder(1);
			postProcessActor->SetUnBound(true);
			postProcessActor->SetHiddenInGame(true);

			auto postProcessComponent = postProcessActor->GetPostProcessComponent();

			auto mat = postProcessComponent->GetMaterials()[0];
			mat->AlbedoColor = { 21.0f / 255.0f, 105.0f / 255.0f, 199.0f / 255.0f };
			mat->OpacityTexture = resourceManager.GetTexture("assets/textures/bump_reverse.png");
			mat->TexCoordMultiplier = { 3.0f, 3.0f * (1080.0f / 1920.0f) };
			mat->OpacityMultiplier = 0.3f;


			m_PlayerPawn->ShieldPostProcess = postProcessActor;
		}
		



	}

	void Update(float deltaTime) override
	{
		Level::Update(deltaTime);

		m_DroneSpawnTimer.Tick();

		if(m_DroneSpawnTimer.GetElapsedTime() >= m_DroneSpawnInterval)
		{
			m_DroneSpawnTimer.Reset();
			for (int j = 0; j < 5; j++)
			{
				auto drone = SpawnActor<Drone>({ "Drone" }).lock();
				drone->SetPosition(Crystal::Vector3::RandomPositionInSphere({ 0.0f, 0.0f, 50000.0f }, 10000.0f));

				auto droneController = SpawnActor<DroneAIController>({}).lock();
				droneController->Possess(drone);
			}
		}



		if(m_Cruiser->GetHealth() <= 0.0f)
		{
			auto world = GetWorld().lock();
			world->PopLevel();
		}

		if (m_PlayerPawn->GetHealth() <= 0.0f)
		{
			auto world = GetWorld().lock();
			world->PopLevel();
		}
		
		
	}


	void OnLevelClosed(Crystal::Shared<Level> nextLevel) override
	{
		Level::OnLevelClosed(nextLevel);

		m_Cruiser->OnCruiserMoving();

		auto hud = Crystal::Cast<MyHUD>(m_HUD);
		hud->HideCruiserHud();

		auto playerStartActor = Crystal::Cast<Crystal::PlayerStartActor>(nextLevel->GetActorByClass("PlayerStartActor"));
		m_Player->SetPosition(playerStartActor->GetPosition());


		nextLevel->SetPlayer(m_Player);
		nextLevel->SetPlayerController(m_PlayerControllers[0]);
		nextLevel->SetHUD(m_HUD);

		MoveActorToLevel(m_Player, nextLevel);
		MoveActorToLevel(m_PlayerControllers[0], nextLevel);
		MoveActorToLevel(m_HUD, nextLevel);
		auto inventory = GetActorByClass("Inventory");
		MoveActorToLevel(inventory.lock(), nextLevel);


		ClearActors();
	}

private:
	Crystal::Timer m_DroneSpawnTimer;
	Crystal::Shared<Cruiser> m_Cruiser;
	Crystal::Shared<MyPlayerPawn> m_PlayerPawn;
	const float m_DroneSpawnInterval = 10.0f;
};
