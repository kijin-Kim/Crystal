#pragma once
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/ParticleComponent.h"
#include "Crystal/Renderer/Pipelines/RenderPipelines/GeometryStaticPipeline.h"

class Asteroid : public Crystal::Actor
{
	SERIALIZE_PROPERTIES
	{
		boost::serialization::base_object<Crystal::Actor>(*this);
	}

public:
	Asteroid() = default;
	~Asteroid() override = default;

	void Initialize() override
	{
		auto material = Crystal::CreateShared<Crystal::Material>();
		material->BlendMode = Crystal::EBlendMode::BM_Opaque;
		material->ShadingModel = Crystal::EShadingModel::SM_Lit;


		auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
		staticMeshComponent->AddMaterial(material);

		m_MainComponent = staticMeshComponent;


		int randomNumber = rand() % 6;
		auto& resourceManager = Crystal::ResourceManager::Instance();
		switch (randomNumber)
		{
		case 0:
			material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_6_A.tga");
			material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_6_R.tga");
			material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_6_N.tga");
			staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/Asteroid_6.fbx"));
			break;
		case 1:
			material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_7_A.tga");
			material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_7_R.tga");
			material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_7_N.tga");
			staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/Asteroid_7.fbx"));
			break;
		case 2:
			material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_8_A.tga");
			material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_8_R.tga");
			material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_8_N.tga");
			staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/Asteroid_8.fbx"));
			break;
		case 3:
			material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_9_A.tga");
			material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_9_R.tga");
			material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_9_N.tga");
			staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/Asteroid_9.fbx"));
			break;
		case 4:
			material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_10_A.tga");
			material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_10_R.tga");
			material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_10_N.tga");
			staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/Asteroid_10.fbx"));
			break;
		case 5:
			material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_11_A.tga");
			material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_11_R.tga");
			material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_11_N.tga");
			staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/Asteroid_11.fbx"));
			break;
		}


		auto randomScale = rand() % 10 + 1;

		m_MainComponent->RotateRoll(rand() % 360);
		m_MainComponent->RotatePitch(rand() % 360);
		m_MainComponent->RotateYaw(rand() % 360);
		m_MainComponent->SetUnitScale(randomScale / 50.0f);
		m_MainComponent->SetMass(5000.0f * randomScale);
	}

	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override
	{
		Destroy();
	}


	STATIC_TYPE_IMPLE(Asteroid)
};

class HealAsteroid : public Crystal::Actor
{
public:
	HealAsteroid() = default;
	~HealAsteroid() override = default;


	void Initialize() override
	{
		auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
		boundingSphereComponent->SetRadius(40.0f);

		m_MainComponent = boundingSphereComponent;

		auto& resourceManager = Crystal::ResourceManager::Instance();


		auto material = Crystal::CreateShared<Crystal::Material>();
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_5_A.tga");
		material->MetallicTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_5_M.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_5_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_5_N.tga");
		material->EmissiveTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_5_E.tga");

		auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
		staticMeshComponent->AddMaterial(std::move(material));
		staticMeshComponent->AttachTo(m_MainComponent);
		staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/Asteroid_5.fbx"));


		auto randomScale = rand() % 3 + 1;

		m_MainComponent->RotateRoll(rand() % 360);
		m_MainComponent->RotatePitch(rand() % 360);
		m_MainComponent->RotateYaw(rand() % 360);
		m_MainComponent->SetUnitScale(randomScale);
		m_MainComponent->SetMass(5000.0f * randomScale);
	}

	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override
	{
		Destroy();
	}

	STATIC_TYPE_IMPLE(HealAsteroid)
};

class PowerAsteroid : public Crystal::Actor
{
public:
	PowerAsteroid() = default;
	~PowerAsteroid() override = default;


	void Initialize() override
	{
		auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
		boundingSphereComponent->SetRadius(40.0f);

		m_MainComponent = boundingSphereComponent;

		auto& resourceManager = Crystal::ResourceManager::Instance();


		auto material = Crystal::CreateShared<Crystal::Material>();
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_2_A.tga");
		material->MetallicTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_2_M.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_2_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_2_N.tga");
		material->EmissiveTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_2_E.tga");
		material->ShadingModel = Crystal::EShadingModel::SM_Lit;
		material->BlendMode = Crystal::EBlendMode::BM_Opaque;

		auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
		staticMeshComponent->AddMaterial(std::move(material));
		staticMeshComponent->AttachTo(m_MainComponent);
		staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/Asteroid_2.fbx"));


		auto randomScale = rand() % 3 + 1;

		m_MainComponent->RotateRoll(rand() % 360);
		m_MainComponent->RotatePitch(rand() % 360);
		m_MainComponent->RotateYaw(rand() % 360);
		m_MainComponent->SetUnitScale(randomScale);
		m_MainComponent->SetMass(5000.0f * randomScale);
	}

	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override
	{
		Destroy();
	}

	STATIC_TYPE_IMPLE(PowerAsteroid)
};


class ShieldAsteroid : public Crystal::Actor
{
public:
	ShieldAsteroid() = default;
	~ShieldAsteroid() override = default;


	void Initialize() override
	{
		auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
		boundingSphereComponent->SetRadius(40.0f);

		m_MainComponent = boundingSphereComponent;

		auto& resourceManager = Crystal::ResourceManager::Instance();


		auto material = Crystal::CreateShared<Crystal::Material>();
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_3_A.tga");
		material->MetallicTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_3_M.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_3_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_3_N.tga");
		material->EmissiveTexture = resourceManager.GetTexture("assets/textures/Asteroid/AstOre_3_E.tga");

		auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
		staticMeshComponent->AddMaterial(std::move(material));
		staticMeshComponent->AttachTo(m_MainComponent);
		staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/Asteroid_3.fbx"));


		auto randomScale = rand() % 3 + 1;

		m_MainComponent->RotateRoll(rand() % 360);
		m_MainComponent->RotatePitch(rand() % 360);
		m_MainComponent->RotateYaw(rand() % 360);
		m_MainComponent->SetUnitScale(randomScale);
		m_MainComponent->SetMass(5000.0f * randomScale);
	}

	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override
	{
		Destroy();
	}

	STATIC_TYPE_IMPLE(ShieldAsteroid)
};
