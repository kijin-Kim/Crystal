#pragma once
#include "Asteroid.h"
#include "Cruiser.h"
#include "Kraken.h"
#include "LevelLandscapeActor.h"
#include "MyHUD.h"
#include "MyPlayerPawn.h"
#include "SpaceStation.h"
#include "Stargate.h"
#include "Sun.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "Crystal/GamePlay/Objects/Actors/BoundingOrientedBoxActor.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Scene.h"

class MainLevel : public Crystal::Level
{
public:
	MainLevel() = default;
	~MainLevel() override = default;

	void Initialize() override
	{
		Level::Initialize();

		auto& resourceManager = Crystal::ResourceManager::Instance();
		auto& scene = GetScene();


		scene->PanoramaCubeColorTexture = resourceManager.GetTexture("assets/textures/cubemaps/T_Cube_Skybox_1.hdr").lock();
		scene->CubemapColorTexture = Crystal::CreateShared<Crystal::Texture>(1024, 1024, 6, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

		scene->PanoramaCubeAlphaTexture = resourceManager.GetTexture("assets/textures/cubemaps/T_Cube_Skybox_1_Alpha.hdr").lock();
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

		scene->SkyboxConfiguration.RedLevel = 1.0f;
		scene->SkyboxConfiguration.GreenLevel = 1.0f;
		scene->SkyboxConfiguration.BlueLevel = 1.0f;
		scene->SkyboxConfiguration.Power = 0.75f;
		scene->SkyboxConfiguration.Saturation = 1.1f;
		scene->SkyboxConfiguration.NearStarDesaturation = 0.0f;
		scene->SkyboxConfiguration.FarStarDesaturation = 0.3f;
		scene->SkyboxConfiguration.AdditiveColor = { 0.008364, 0.012143, 0.026042, 1.000000 };


		auto playerStartActor = SpawnActor<Crystal::PlayerStartActor>({ "" }).lock();
		playerStartActor->SetPosition({ -10000.0f, 10000.0f, 0.0f });

		SpawnActor<Crystal::PlayerStartActor>({});

		m_HUD = SpawnActor<MyHUD>({ "" }).lock();
		m_Player = SpawnActor<MyPlayerPawn>({ "MyPlayerPawn" }).lock();
		m_Player->SetPosition({-10000.0f, 10000.0f, 0.0f});
		auto playerController = SpawnActor<Crystal::PlayerController>({}).lock();
		playerController->Possess(m_Player);
		playerController->EnableModeSwitching(true, Crystal::Keyboard::M);


		
		auto stargate = SpawnActor<Stargate>({""}).lock();
		stargate->SetPosition({ 0.0f, 0.0f, 40000.0f });


		auto spaceStation = SpawnActor<SpaceStation>({ "" }).lock();
		spaceStation->SetPosition(Crystal::Vector3::Zero);


		{
			auto sun = SpawnActor<Sun>({ "Sun" }).lock();
			sun->SetPosition({ +30000.0f, 50000.0f, 0.0f });

			auto lightComponent = Crystal::Cast<Crystal::LightComponent>(sun->GetComponentByClass("DirectionalLightComponent"));
			lightComponent->SetLightColor({ 1.0f, 1.0f, 1.0f });

			lightComponent->SetLightIntensity(7.0f);
			lightComponent->RotateYaw(-90.0f);
			lightComponent->RotatePitch(45.0f);
			lightComponent->SetCastShadow(true);

			auto sunMesh = Crystal::Cast<Crystal::StaticMeshComponent>(sun->GetComponentByClass("StaticMeshComponent"));
			auto sunMat = sunMesh->GetMaterial(0);
			sunMat->EmissiveColor = { 1.0f * 7.0f, 1.0f * 7.0f, 90.0f / 255.0f * 7.0f };
		}


		{
			auto lightActor = SpawnActor<Crystal::DirectionalLightActor>({ "LightActor2" }).lock();
			auto lightComponent = Crystal::Cast<Crystal::LightComponent>(lightActor->GetComponentByClass("DirectionalLightComponent"));
			lightComponent->SetLightColor({ 1.0f, 1.0f, 1.0f });
			lightComponent->SetLightIntensity(2.0f);
			lightComponent->RotateYaw(135.0f);
			lightComponent->RotatePitch(30.0f);
			lightComponent->SetCastShadow(false);
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

			auto myPlayerPawn = Crystal::Cast<MyPlayerPawn>(GetActorByClass("MyPlayerPawn"));
			myPlayerPawn->DamagedPostProcessActor = postProcessActor;
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

			auto myPlayerPawn = Crystal::Cast<MyPlayerPawn>(GetActorByClass("MyPlayerPawn"));
			myPlayerPawn->HealPostProcessActor = postProcessActor;
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


			auto myPlayerPawn = Crystal::Cast<MyPlayerPawn>(GetActorByClass("MyPlayerPawn"));
			myPlayerPawn->ShieldPostProcess = postProcessActor;
		}

	}

	void OnLevelOpened(Crystal::Shared<Level> lastLevel) override
	{
		Level::OnLevelOpened(lastLevel);
	}

	void OnLevelClosed(std::shared_ptr<Level> nextLevel) override
	{
		Level::OnLevelClosed(nextLevel);
		if(nextLevel->GetObjectName() == "ShopLevel" || nextLevel->GetObjectName() == "LevelClearedLevel")
		{
			auto inventory = GetActorByClass("Inventory");
			MoveActorToLevel(inventory.lock(), nextLevel);
		}
		else if(nextLevel->GetObjectName() == "StargateLevel")
		{
		}
		else
		{
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
			
		}

	}

};

