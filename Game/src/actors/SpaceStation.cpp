#include "SpaceStation.h"

#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"


void SpaceStation::Initialize()
{
	Actor::Initialize();

	auto coreMeshBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
	coreMeshBoxComponent->SetExtents({2000.0f / 2.0f, 10000.0f / 2.0f, 2000.0f / 2.0f});
	coreMeshBoxComponent->SetInverseMass(0.0f);

	m_MainComponent = coreMeshBoxComponent;


	auto& resourceManager = Crystal::ResourceManager::Instance();


	auto baseMaterial1 = Crystal::CreateShared<Crystal::Material>();
	baseMaterial1->AlbedoTexture = resourceManager.GetTexture("assets/textures/T_MSS_1_Dif.tga");
	baseMaterial1->MetallicTexture = resourceManager.GetTexture("assets/textures/T_MSS_1_Metal.tga");
	baseMaterial1->RoughnessTexture = resourceManager.GetTexture("assets/textures/T_MSS_1_Rough.tga");
	baseMaterial1->EmissiveTexture = resourceManager.GetTexture("assets/textures/T_MSS_1_Glow.tga");
	baseMaterial1->NormalTexture = resourceManager.GetTexture("assets/textures/T_MSS_1_Norm.tga");

	auto baseMaterial2 = Crystal::CreateShared<Crystal::Material>();
	baseMaterial2->AlbedoTexture = resourceManager.GetTexture("assets/textures/T_MSS_2_Dif.tga");
	baseMaterial2->MetallicTexture = resourceManager.GetTexture("assets/textures/T_MSS_2_Metal.tga");
	baseMaterial2->RoughnessTexture = resourceManager.GetTexture("assets/textures/T_MSS_2_Rough.tga");
	baseMaterial2->NormalTexture = resourceManager.GetTexture("assets/textures/T_MSS_2_Norm.tga");


	auto towerMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("TowerMeshComponent");
	towerMeshComponent->SetLocalPosition({0.0f, 2300.0f, 0.0f});
	towerMeshComponent->AddMaterial(baseMaterial1);
	towerMeshComponent->AttachTo(m_MainComponent);
	towerMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Tower.fbx"));
	towerMeshComponent->RotateYaw(180.0f);
	towerMeshComponent->RotatePitch(90.0f);


	float angle = 0.0f;
	for (int i = 0; i < 4; i++)
	{
		auto habitatMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("HabitatMeshComponent");
		habitatMeshComponent->SetLocalPosition({0.0f, 0.0f, -1620.0f});
		habitatMeshComponent->AddMaterial(baseMaterial1);
		habitatMeshComponent->AttachTo(towerMeshComponent);
		habitatMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Habitat.fbx"));
		habitatMeshComponent->RotateRoll(angle);


		auto habitatBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		habitatBoxComponent->SetLocalPosition({2200.0f, 0.0f, 0.0f});
		habitatBoxComponent->SetExtents({1000.0f / 2.0f, 3000.0f / 2.0f, 750.0f / 2.0f});
		habitatBoxComponent->SetInverseMass(0.0f);
		habitatBoxComponent->AttachTo(habitatMeshComponent);

		angle += 90.0f;
	}


	angle = 0.0f;
	for (int i = 0; i < 4; i++)
	{
		auto meshComponent = CreateComponent<Crystal::StaticMeshComponent>("SubTowerMeshComponent");
		meshComponent->RotateRoll(angle);
		meshComponent->SetLocalPosition(Crystal::Vector3::RotateQuaternion({0.0f, -822.0f, -400.0f}, meshComponent->GetRotationQuat()));
		meshComponent->AddMaterial(baseMaterial1);
		meshComponent->AttachTo(towerMeshComponent);
		meshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_TowerB.fbx"));

		auto subTowerBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		subTowerBoxComponent->SetLocalPosition({0.0f, -600.0f / 2.0f, 2300.0f});
		subTowerBoxComponent->SetExtents({600.0f / 2.0f, 650.0f / 2.0f, 7500.0f / 2.0f});
		subTowerBoxComponent->SetInverseMass(0.0f);
		subTowerBoxComponent->AttachTo(meshComponent);

		angle += 90.0f;
	}

	angle = 0.0f;
	for (int i = 0; i < 4; i++)
	{
		auto meshComponent = CreateComponent<Crystal::StaticMeshComponent>("ConnectMeshComponent");
		meshComponent->RotateRoll(angle);
		meshComponent->SetLocalPosition(Crystal::Vector3::RotateQuaternion({0.0f, -1450.0f, -1620.0f}, meshComponent->GetRotationQuat()));
		meshComponent->AddMaterial(baseMaterial1);
		meshComponent->AttachTo(towerMeshComponent);
		meshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Connect.fbx"));
		angle += 90.0f;
	}

	angle = 0.0f;
	for (int i = 0; i < 4; i++)
	{
		auto meshComponent = CreateComponent<Crystal::StaticMeshComponent>("ModuleBMeshComponent");
		meshComponent->RotateRoll(angle);
		meshComponent->SetLocalPosition(Crystal::Vector3::RotateQuaternion({+1640.0f, 0.0f, -300.0f}, meshComponent->GetRotationQuat()));
		meshComponent->AddMaterial(baseMaterial1);
		meshComponent->AttachTo(towerMeshComponent);
		meshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_ModuleB.fbx"));
		angle += 90.0f;
	}

	angle = 0.0f;
	for (int i = 0; i < 2; i++)
	{
		auto meshComponent = CreateComponent<Crystal::StaticMeshComponent>("ControlMeshComponent");
		meshComponent->RotateRoll(angle);
		meshComponent->SetLocalPosition(Crystal::Vector3::RotateQuaternion({0.0f, -129.0f, -2406.0f}, meshComponent->GetRotationQuat()));
		meshComponent->AddMaterial(baseMaterial1);
		meshComponent->AttachTo(towerMeshComponent);
		meshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Control.fbx"));


		auto controlMeshBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		controlMeshBoxComponent->SetLocalPosition({ 0.0f, -2600.0f / 2.0f, -300.0f });
		controlMeshBoxComponent->SetExtents({ 1400.0f / 2.0f, 2600.0f / 2.0f, 1300.0f / 2.0f });
		controlMeshBoxComponent->SetInverseMass(0.0f);
		controlMeshBoxComponent->AttachTo(meshComponent);

		angle += 180.0f;
	}


	{
		auto scienceMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("ScienceMeshComponent");
		scienceMeshComponent->SetLocalPosition({0.0f, +2860.0f, -1770.0f});
		scienceMeshComponent->AddMaterial(baseMaterial1);
		scienceMeshComponent->AttachTo(towerMeshComponent);
		scienceMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Science.fbx"));


		auto scienceMeshBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		scienceMeshBoxComponent->SetLocalPosition({0.0f, 3700.0f / 2.0f, 0.0f});
		scienceMeshBoxComponent->SetExtents({2600.0f / 2.0f, 3700.0f / 2.0f, 800.0f / 2.0f});
		scienceMeshBoxComponent->SetInverseMass(0.0f);
		scienceMeshBoxComponent->AttachTo(scienceMeshComponent);


		{
			auto communicationMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("CommunicationMeshComponent");
			communicationMeshComponent->SetLocalPosition({0.0f, 1290.0f, 0.0f});
			communicationMeshComponent->AddMaterial(baseMaterial2);
			communicationMeshComponent->AttachTo(scienceMeshComponent);
			communicationMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_2_Communications.fbx"));


			auto connectMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("ConnectMeshComponent");
			connectMeshComponent->SetLocalPosition({0.0f, 4110.0f, 0.0f});
			connectMeshComponent->AddMaterial(baseMaterial1);
			connectMeshComponent->AttachTo(scienceMeshComponent);
			connectMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Connect.fbx"));

			{
				auto storageMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StorageMeshComponent");
				storageMeshComponent->AddMaterial(baseMaterial1);
				storageMeshComponent->AttachTo(connectMeshComponent);
				storageMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Storage.fbx"));

				auto storageMeshBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
				storageMeshBoxComponent->SetLocalPosition({ 0.0f, 1500.0f / 2.0f , 0.0f});
				storageMeshBoxComponent->SetExtents({1100.0f / 2.0f, 1500.0f / 2.0f, 2000.0f / 2.0f});
				storageMeshBoxComponent->SetInverseMass(0.0f);
				storageMeshBoxComponent->AttachTo(storageMeshComponent);
			}
		}
	}


	{
		auto meshComponent = CreateComponent<Crystal::StaticMeshComponent>("EnergyMeshComponent");
		meshComponent->SetLocalPosition({0.0f, 8220.0f, -1770.0f});
		meshComponent->AddMaterial(baseMaterial2);
		meshComponent->AttachTo(towerMeshComponent);
		meshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_2_Energy.fbx"));

		auto energyMeshBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		energyMeshBoxComponent->SetLocalPosition({ 0.0f, 1500.0f / 2.0f , 0.0f});
		energyMeshBoxComponent->SetExtents({1800.0f / 2.0f, 1500.0f / 2.0f, 2000.0f / 2.0f});
		energyMeshBoxComponent->SetInverseMass(0.0f);
		energyMeshBoxComponent->AttachTo(meshComponent);
	}


	{
		auto bridgeMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("BridgeMeshComponent");
		bridgeMeshComponent->SetLocalPosition({0.0f, -2649.0f, -1413.0f});
		bridgeMeshComponent->AddMaterial(baseMaterial1);
		bridgeMeshComponent->AttachTo(towerMeshComponent);
		bridgeMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Bridge.fbx"));

		auto bridgeMeshesBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		bridgeMeshesBoxComponent->SetLocalPosition({ 0.0f,  -(1200.0f / 2.0f * 3.0f), 0.0f });
		bridgeMeshesBoxComponent->SetExtents({ 2500.0f / 2.0f, 1200.0f / 2.0f * 3.0f, 1500.0f / 2.0f });
		bridgeMeshesBoxComponent->SetInverseMass(0.0f);
		bridgeMeshesBoxComponent->AttachTo(bridgeMeshComponent);


		{
			auto antennaMeshComponent1 = CreateComponent<Crystal::StaticMeshComponent>("AntennaMeshComponent1");
			antennaMeshComponent1->SetLocalPosition({-1310.0f, -615.0f, -90.0f});
			antennaMeshComponent1->AddMaterial(baseMaterial1);
			antennaMeshComponent1->AttachTo(bridgeMeshComponent);
			antennaMeshComponent1->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Antennas.fbx"));

			auto antennasMesh1BoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
			antennasMesh1BoxComponent->SetLocalPosition({ -3000.0f / 2.0f, 0.0f , 0.0f });
			antennasMesh1BoxComponent->SetExtents({ 3000.0f / 2.0f, 650.0f / 2.0f, 500.0f / 2.0f });
			antennasMesh1BoxComponent->SetInverseMass(0.0f);
			antennasMesh1BoxComponent->AttachTo(antennaMeshComponent1);

			auto antennaMeshComponent2 = CreateComponent<Crystal::StaticMeshComponent>("AntennaMeshComponent2");
			antennaMeshComponent2->SetLocalPosition({+1310.0f, -615.0f, -90.0f});
			antennaMeshComponent2->AddMaterial(baseMaterial1);
			antennaMeshComponent2->AttachTo(bridgeMeshComponent);
			antennaMeshComponent2->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Antennas.fbx"));
			antennaMeshComponent2->RotateRoll(180.0f);

			auto antennasMesh2BoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
			antennasMesh2BoxComponent->SetLocalPosition({ -3000.0f / 2.0f, 0.0f , 0.0f });
			antennasMesh2BoxComponent->SetExtents({ 3000.0f / 2.0f, 650.0f / 2.0f, 500.0f / 2.0f });
			antennasMesh2BoxComponent->SetInverseMass(0.0f);
			antennasMesh2BoxComponent->AttachTo(antennaMeshComponent2);

			



			auto bridgeMeshComponent2 = CreateComponent<Crystal::StaticMeshComponent>("BridgeMeshComponent2");
			bridgeMeshComponent2->SetLocalPosition({0.0f, -1235.0f, 0.0f});
			bridgeMeshComponent2->AddMaterial(baseMaterial1);
			bridgeMeshComponent2->AttachTo(bridgeMeshComponent);
			bridgeMeshComponent2->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Bridge.fbx"));

			{
				auto bridgeMeshComponent3 = CreateComponent<Crystal::StaticMeshComponent>("BridgeMeshComponent3");
				bridgeMeshComponent3->SetLocalPosition({0.0f, -1235.0f, 0.0f});
				bridgeMeshComponent3->AddMaterial(baseMaterial1);
				bridgeMeshComponent3->AttachTo(bridgeMeshComponent2);
				bridgeMeshComponent3->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Bridge.fbx"));


				{
					auto energyMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("EnergyMeshComponent2");
					energyMeshComponent->SetLocalPosition({0.0f, -3675.0f, 0.0f});
					energyMeshComponent->AddMaterial(baseMaterial1);
					energyMeshComponent->AttachTo(bridgeMeshComponent3);
					energyMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Energy.fbx"));

					auto energyMeshBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
					energyMeshBoxComponent->SetLocalPosition({ 0.0f, +2400.0f / 2.0f , 0.0f });
					energyMeshBoxComponent->SetExtents({ 1000.0f / 2.0f, 2400.0f / 2.0f, 1000.0f / 2.0f });
					energyMeshBoxComponent->SetInverseMass(0.0f);
					energyMeshBoxComponent->AttachTo(energyMeshComponent);

					{
						auto moduleComponent1 = CreateComponent<Crystal::StaticMeshComponent>("ModuleMeshComponent2");
						moduleComponent1->SetLocalPosition({-510.0f, 830.0f, 0.0f});
						moduleComponent1->AddMaterial(baseMaterial1);
						moduleComponent1->AttachTo(energyMeshComponent);
						moduleComponent1->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Modules.fbx"));

						auto module1BoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
						module1BoxComponent->SetLocalPosition({ -1800.0f / 2.0f, 0.0f, 0.0f });
						module1BoxComponent->SetExtents({ 1800.0f / 2.0f, 700.0f / 2.0f, 700.0f / 2.0f });
						module1BoxComponent->SetInverseMass(0.0f);
						module1BoxComponent->AttachTo(moduleComponent1);


						auto moduleComponent2 = CreateComponent<Crystal::StaticMeshComponent>("ModuleMeshComponent2");
						moduleComponent2->SetLocalPosition({510.0f, 830.0f, 0.0f});
						moduleComponent2->AddMaterial(baseMaterial1);
						moduleComponent2->AttachTo(energyMeshComponent);
						moduleComponent2->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Modules.fbx"));
						moduleComponent2->RotateRoll(180.0f);

						auto module2BoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
						module2BoxComponent->SetLocalPosition({ -1800.0f / 2.0f, 0.0f , 0.0f });
						module2BoxComponent->SetExtents({ 1800.0f / 2.0f, 700.0f / 2.0f, 700.0f / 2.0f });
						module2BoxComponent->SetInverseMass(0.0f);
						module2BoxComponent->AttachTo(moduleComponent2);
					}

					auto containerMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("EnergyMeshComponent2");
					containerMeshComponent->SetLocalPosition({0.0f, -3670.0f, 0.0f});
					containerMeshComponent->AddMaterial(baseMaterial2);
					containerMeshComponent->AttachTo(bridgeMeshComponent3);
					containerMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_2_Container.fbx"));

					auto containerMeshBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
					containerMeshBoxComponent->SetLocalPosition({ 0.0f, -1800.0f / 2.0f , 0.0f });
					containerMeshBoxComponent->SetExtents({ 1300.0f / 2.0f, 1800.0f / 2.0f, 1400.0f / 2.0f });
					containerMeshBoxComponent->SetInverseMass(0.0f);
					containerMeshBoxComponent->AttachTo(containerMeshComponent);
				}
			}
		}
	}


	{
		auto solarArrayMeshComponent1 = CreateComponent<Crystal::StaticMeshComponent>("SolarArrayMeshComponent1");
		solarArrayMeshComponent1->SetLocalPosition({-1310.0f, -5735.0f, -1505.0f});
		solarArrayMeshComponent1->AddMaterial(baseMaterial2);
		solarArrayMeshComponent1->AttachTo(towerMeshComponent);
		solarArrayMeshComponent1->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_2_SolarArray.fbx"));
		solarArrayMeshComponent1->RotatePitch(73.0f);

		auto solarArray1BoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		solarArray1BoxComponent->SetLocalPosition({ -10400.0f / 2.0f, 0.0f, 0.0f });
		solarArray1BoxComponent->SetExtents({ 10400.0f / 2.0f, 2000.0f / 2.0f, 10.0f / 2.0f });
		solarArray1BoxComponent->SetInverseMass(0.0f);
		solarArray1BoxComponent->AttachTo(solarArrayMeshComponent1);

		auto solarArrayMeshComponent2 = CreateComponent<Crystal::StaticMeshComponent>("SolarArrayMeshComponent2");
		solarArrayMeshComponent2->SetLocalPosition({1310.0f, -5735.0f, -1505.0f});
		solarArrayMeshComponent2->AddMaterial(baseMaterial2);
		solarArrayMeshComponent2->AttachTo(towerMeshComponent);
		solarArrayMeshComponent2->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_2_SolarArray.fbx"));
		solarArrayMeshComponent2->RotateRoll(180.0f);
		solarArrayMeshComponent2->RotatePitch(-73.0f);

		auto solarArray2BoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		solarArray2BoxComponent->SetLocalPosition({ -10400.0f / 2.0f, 0.0f, 0.0f });
		solarArray2BoxComponent->SetExtents({ 10400.0f / 2.0f, 2000.0f / 2.0f, 10.0f / 2.0f });
		solarArray2BoxComponent->SetInverseMass(0.0f);
		solarArray2BoxComponent->AttachTo(solarArrayMeshComponent2);
	}

	{
		auto sensorArrayMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("SensorArrayMeshComponent");
		sensorArrayMeshComponent->SetLocalPosition({-5645.0f, 0.0f, -1640.0f});
		sensorArrayMeshComponent->AddMaterial(baseMaterial2);
		sensorArrayMeshComponent->AttachTo(towerMeshComponent);
		sensorArrayMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_2_SensorArray.fbx"));

		auto sensorArrayMeshBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		sensorArrayMeshBoxComponent->SetLocalPosition({ 2800.0f / 2.0f , 0.0f, 0.0f });
		sensorArrayMeshBoxComponent->SetExtents({ 2800.0f / 2.0f, 800.0f / 2.0f, 2200.0f / 2.0f });
		sensorArrayMeshBoxComponent->SetInverseMass(0.0f);
		sensorArrayMeshBoxComponent->AttachTo(sensorArrayMeshComponent);

		{
			auto serviceMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("ServiceMeshComponent");
			serviceMeshComponent->SetLocalPosition({415.0f, 0.0f, 0.0f});
			serviceMeshComponent->AddMaterial(baseMaterial2);
			serviceMeshComponent->AttachTo(sensorArrayMeshComponent);
			serviceMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_2_Service.fbx"));

			auto serviceMeshBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
			serviceMeshBoxComponent->SetLocalPosition({ -740.0f / 2.0f , 0.0f, 0.0f });
			serviceMeshBoxComponent->SetExtents({ 740.0f / 2.0f, 2200.0f / 2.0f, 2200.0f / 2.0f });
			serviceMeshBoxComponent->SetInverseMass(0.0f);
			serviceMeshBoxComponent->AttachTo(serviceMeshComponent);
		}
	}

	{
		auto connectionMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("ConnectionMeshComponent");
		connectionMeshComponent->SetLocalPosition({2745.0f, 0.0f, -1650.0f});
		connectionMeshComponent->AddMaterial(baseMaterial1);
		connectionMeshComponent->AttachTo(towerMeshComponent);
		connectionMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Connections.fbx"));

		auto connectionMeshBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		connectionMeshBoxComponent->SetLocalPosition({ +2500.0f / 2.0f , 0.0f, -350.0f / 2.0f });
		connectionMeshBoxComponent->SetExtents({ 2500.0f / 2.0f, 3000.0f / 2.0f, 1300.0f / 2.0f });
		connectionMeshBoxComponent->SetInverseMass(0.0f);
		connectionMeshBoxComponent->AttachTo(connectionMeshComponent);
		



		auto hangarMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("HangarMeshComponent");
		hangarMeshComponent->SetLocalPosition({3490.0f, -360.0f, 0.0f});
		hangarMeshComponent->AddMaterial(baseMaterial1);
		hangarMeshComponent->AttachTo(connectionMeshComponent);
		hangarMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Hangars.fbx"));

		auto hangarMeshBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		hangarMeshBoxComponent->SetLocalPosition({ 0.0f, -1800.0f / 2.0f, 0.0f });
		hangarMeshBoxComponent->SetExtents({ 1300.0f / 2.0f, 1800.0f / 2.0f, 400.0f / 2.0f });
		hangarMeshBoxComponent->SetInverseMass(0.0f);
		hangarMeshBoxComponent->AttachTo(hangarMeshComponent);




		auto hangarMeshComponent2 = CreateComponent<Crystal::StaticMeshComponent>("HangarMeshComponent2");
		hangarMeshComponent2->SetLocalPosition({3490.0f, 360.0f, 0.0f});
		hangarMeshComponent2->AddMaterial(baseMaterial1);
		hangarMeshComponent2->AttachTo(connectionMeshComponent);
		hangarMeshComponent2->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Hangars.fbx"));
		hangarMeshComponent2->RotatePitch(180.0f);

		auto hangarMeshBox2Component = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		hangarMeshBox2Component->SetLocalPosition({ 0.0f, -1800.0f / 2.0f, 0.0f });
		hangarMeshBox2Component->SetExtents({ 1300.0f / 2.0f, 1800.0f / 2.0f, 400.0f / 2.0f });
		hangarMeshBox2Component->SetInverseMass(0.0f);
		hangarMeshBox2Component->AttachTo(hangarMeshComponent2);


		auto hangarMeshComponent3 = CreateComponent<Crystal::StaticMeshComponent>("HangarMeshComponent3");
		hangarMeshComponent3->SetLocalPosition({5085.0f, -360.0f, 0.0f});
		hangarMeshComponent3->AddMaterial(baseMaterial1);
		hangarMeshComponent3->AttachTo(connectionMeshComponent);
		hangarMeshComponent3->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Hangars.fbx"));


		auto hangarMeshBox3Component = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		hangarMeshBox3Component->SetLocalPosition({ 0.0f, -1800.0f / 2.0f, 0.0f });
		hangarMeshBox3Component->SetExtents({ 1300.0f / 2.0f, 1800.0f / 2.0f, 400.0f / 2.0f });
		hangarMeshBox3Component->SetInverseMass(0.0f);
		hangarMeshBox3Component->AttachTo(hangarMeshComponent3);


		auto hangarMeshComponent4 = CreateComponent<Crystal::StaticMeshComponent>("HangarMeshComponent4");
		hangarMeshComponent4->SetLocalPosition({5085, 360.0f, 0.0f});
		hangarMeshComponent4->AddMaterial(baseMaterial1);
		hangarMeshComponent4->AttachTo(connectionMeshComponent);
		hangarMeshComponent4->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MSS_1_Hangars.fbx"));
		hangarMeshComponent4->RotatePitch(180.0f);


		auto hangarMeshBox4Component = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		hangarMeshBox4Component->SetLocalPosition({ 0.0f, -1800.0f / 2.0f, 0.0f });
		hangarMeshBox4Component->SetExtents({ 1300.0f / 2.0f, 1800.0f / 2.0f, 400.0f / 2.0f });
		hangarMeshBox4Component->SetInverseMass(0.0f);
		hangarMeshBox4Component->AttachTo(hangarMeshComponent4);
	}
}

