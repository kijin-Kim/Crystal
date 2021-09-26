#pragma once
#include "AllyDrone.h"
#include "AllyDroneAIController.h"
#include "MineCave.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Scene.h"
#include "Crystal/Resources/ResourceManager.h"


class MiningLevel : public Crystal::Level
{
public:
	MiningLevel() = default;
	~MiningLevel() override = default;


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

		auto& resourceManager = Crystal::ResourceManager::Instance();

		QuestReward reward;
		reward.Gold = 500;
		reward.ItemType.push_back(EItemType::IT_HealPotion);
		reward.ItemType.push_back(EItemType::IT_PowerPotion);
		reward.ItemCount.push_back(1);
		reward.ItemCount.push_back(3);

		Crystal::Cast<MyHUD>(m_HUD)->SetQuestMainText(L"Gather Resources");
		CreateQuest("Destroy Ore Asteroids", { "GreenOreAsteroid","BlueOreAsteroid", "YellowOreAsteroid" }, 20, reward);




		for (int i = 0; i < 5; i++)
		{
			auto allyDrone = SpawnActor<AllyDrone>({""}).lock();
			allyDrone->SetPosition({250.0f + 250.0f * i, 0.0f, 0.0f});
			auto allyDroneController = SpawnActor<AllyDroneAIController>({}).lock();
			allyDroneController->Possess(allyDrone);
		}

		for (int i = 0; i < 5; i++)
		{
			auto allyDrone = SpawnActor<AllyDrone>({""}).lock();
			allyDrone->SetPosition({-250.0f - 250.0f * i, 0.0f, 0.0f});
			auto allyDroneController = SpawnActor<AllyDroneAIController>({}).lock();
			allyDroneController->Possess(allyDrone);
		}



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


		auto asteroid = SpawnActor<WhiteAsteroid>({}).lock();
		asteroid->SetAsteroidType(3);
		asteroid->SetUnitScale(30.0f * 0.08f);
		asteroid->SetPosition({0.0f, 15000.0f, 15000.0f});

		for (int i = 0; i < 100; i++)
		{
			auto asteroid = SpawnActor<WhiteAsteroid>({}).lock();
			asteroid->SetPosition(Crystal::Vector3::RandomPositionInSphere({0.0f, 15000.0f, 15000.0f}, 20000.0f));
		}


		for (int i = 0; i < 200; i++)
		{
			auto asteroid = SpawnActor<FakeWhiteAsteroid>({}).lock();
			asteroid->SetPosition(Crystal::Vector3::RandomPositionInSphere({0.0f, 15000.0f, 15000.0f}, 20000.0f));
		}

		for (int i = 0; i < 40; i++)
		{
			int randomNumber = rand() % 3;
			switch (randomNumber)
			{
			case 0:
				{
					auto asteroid = SpawnActor<GreenOreAsteroid>({}).lock();
					asteroid->SetPosition(Crystal::Vector3::RandomPositionInSphere({0.0f, 15000.0f, 15000.0f}, 5000.0f));
					break;
				}
			case 1:
				{
					auto asteroid = SpawnActor<BlueOreAsteroid>({}).lock();
					asteroid->SetPosition(Crystal::Vector3::RandomPositionInSphere({0.0f, 15000.0f, 15000.0f}, 5000.0f));
					break;
				}
			case 2:
				{
					auto asteroid = SpawnActor<YellowOreAsteroid>({}).lock();
					asteroid->SetPosition(Crystal::Vector3::RandomPositionInSphere({0.0f, 15000.0f, 15000.0f}, 5000.0f));
					break;
				}
			}
		}
	}

	void OnLevelClosed(Crystal::Shared<Level> nextLevel) override
	{
		Level::OnLevelClosed(nextLevel);

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
};
