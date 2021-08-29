#include "Asteroid.h"

#include "MyPlayerPawn.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/ParticleComponent.h"
#include "Crystal/GamePlay/Components/TextureComponent.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "Crystal/Resources/ResourceManager.h"

BOOST_CLASS_EXPORT(Asteroid)

BOOST_CLASS_EXPORT(HealAsteroid)

BOOST_CLASS_EXPORT(PowerAsteroid)

BOOST_CLASS_EXPORT(ShieldAsteroid)


void Asteroid::Initialize()
{
	auto material = Crystal::CreateShared<Crystal::Material>();
	material->BlendMode = Crystal::EBlendMode::BM_Opaque;
	material->ShadingModel = Crystal::EShadingModel::SM_Lit;

	auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
	boundingSphereComponent->SetRadius(45.0f);
	boundingSphereComponent->SetCollisionType(Crystal::ECollisionType::CT_Block);


	m_MainComponent = boundingSphereComponent;


	auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
	staticMeshComponent->AddMaterial(material);
	staticMeshComponent->AttachTo(m_MainComponent);


	int randomNumber = Crystal::RandomFloatInRange(0, 5);
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


	auto randomScale = Crystal::RandomFloatInRange(6.0f, 10.0f);
	m_MainComponent->RotateRoll(rand() % 360);
	m_MainComponent->RotatePitch(rand() % 360);
	m_MainComponent->RotateYaw(rand() % 360);
	m_MainComponent->SetUnitScale(randomScale);
	m_MainComponent->SetMass(500.0f * randomScale);
}

void WhiteAsteroid::Initialize()
{
	auto material = Crystal::CreateShared<Crystal::Material>();
	material->BlendMode = Crystal::EBlendMode::BM_Opaque;
	material->ShadingModel = Crystal::EShadingModel::SM_Lit;

	auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
	boundingSphereComponent->SetCollisionType(Crystal::ECollisionType::CT_Block);


	m_MainComponent = boundingSphereComponent;


	m_StaticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
	m_StaticMeshComponent->AddMaterial(material);
	m_StaticMeshComponent->AttachTo(m_MainComponent);


	int randomNumber = Crystal::RandomFloatInRange(0, 8);
	auto& resourceManager = Crystal::ResourceManager::Instance();


	switch (randomNumber)
	{
	case 0:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_1_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_1_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_1_N.tga");
		m_StaticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_1.fbx"));
		boundingSphereComponent->SetRadius(215.0f);
		break;
	case 1:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_2_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_2_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_2_N.tga");
		m_StaticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_2.fbx"));
		boundingSphereComponent->SetRadius(617.0f / 2.0f);
		break;
	case 2:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_3_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_3_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_3_N.tga");
		m_StaticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_3.fbx"));
		boundingSphereComponent->SetRadius(663.0f / 2.0f);
		break;
	case 3:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_4_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_4_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_4_N.tga");
		m_StaticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_4.fbx"));
		boundingSphereComponent->SetRadius(3025.0f / 2.0f);
		break;
	case 4:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_5_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_5_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_5_N.tga");
		m_StaticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_5.fbx"));
		boundingSphereComponent->SetRadius(541.0f / 2.0f);
		break;
	case 5:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_6_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_6_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_6_N.tga");
		m_StaticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_6.fbx"));
		boundingSphereComponent->SetRadius(541.0f / 2.0f);
		break;
	case 6:
	case 7:
	case 8:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_8_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_8_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_8_N.tga");
		m_StaticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_8.fbx"));
		boundingSphereComponent->SetRadius(330.0f / 2.0f);
		break;
	}


	auto randomScale = Crystal::RandomFloatInRange(6.0f, 10.0f) * 0.08f;
	m_MainComponent->RotateRoll(rand() % 360);
	m_MainComponent->RotatePitch(rand() % 360);
	m_MainComponent->RotateYaw(rand() % 360);
	m_MainComponent->SetUnitScale(randomScale);
	m_MainComponent->SetMass(500.0f * randomScale);
}

void WhiteAsteroid::SetAsteroidType(int type)
{
	auto material = m_StaticMeshComponent->GetMaterial(0);
	auto& resourceManager = Crystal::ResourceManager::Instance();
	switch (type)
	{
	case 0:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_1_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_1_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_1_N.tga");
		m_StaticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_1.fbx"));
		Crystal::Cast<Crystal::BoundingSphereComponent>(m_MainComponent)->SetRadius(215.0f);
		break;
	case 1:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_2_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_2_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_2_N.tga");
		m_StaticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_2.fbx"));
		Crystal::Cast<Crystal::BoundingSphereComponent>(m_MainComponent)->SetRadius(617.0f / 2.0f);
		break;
	case 2:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_3_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_3_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_3_N.tga");
		m_StaticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_3.fbx"));
		Crystal::Cast<Crystal::BoundingSphereComponent>(m_MainComponent)->SetRadius(663.0f / 2.0f);
		break;
	case 3:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_4_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_4_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_4_N.tga");
		m_StaticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_4.fbx"));
		Crystal::Cast<Crystal::BoundingSphereComponent>(m_MainComponent)->SetRadius(3025.0f / 2.0f);
		break;
	case 4:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_5_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_5_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_5_N.tga");
		m_StaticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_5.fbx"));
		Crystal::Cast<Crystal::BoundingSphereComponent>(m_MainComponent)->SetRadius(541.0f / 2.0f);
		break;
	case 5:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_6_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_6_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_6_N.tga");
		m_StaticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_6.fbx"));
		Crystal::Cast<Crystal::BoundingSphereComponent>(m_MainComponent)->SetRadius(541.0f / 2.0f);
		break;
	case 6:
	case 7:
	case 8:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_8_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_8_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_8_N.tga");
		m_StaticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_8.fbx"));
		Crystal::Cast<Crystal::BoundingSphereComponent>(m_MainComponent)->SetRadius(330.0f / 2.0f);
		break;
	}

}

void FakeAsteroid::Initialize()
{
	auto material = Crystal::CreateShared<Crystal::Material>();
	material->BlendMode = Crystal::EBlendMode::BM_Opaque;
	material->ShadingModel = Crystal::EShadingModel::SM_Lit;

	auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
	staticMeshComponent->AddMaterial(material);
	staticMeshComponent->SetCastShadow(false);

	m_MainComponent = staticMeshComponent;


	int randomNumber = Crystal::RandomFloatInRange(0, 5);
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


	auto randomScale = Crystal::RandomFloatInRange(0.4f, 0.8f);
	m_MainComponent->RotateRoll(rand() % 360);
	m_MainComponent->RotatePitch(rand() % 360);
	m_MainComponent->RotateYaw(rand() % 360);
	m_MainComponent->SetUnitScale(randomScale);
	m_MainComponent->SetMass(500.0f * randomScale);
}

void FakeWhiteAsteroid::Initialize()
{
	auto material = Crystal::CreateShared<Crystal::Material>();
	material->BlendMode = Crystal::EBlendMode::BM_Opaque;
	material->ShadingModel = Crystal::EShadingModel::SM_Lit;

	auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
	staticMeshComponent->AddMaterial(material);
	staticMeshComponent->SetCastShadow(false);

	m_MainComponent = staticMeshComponent;


	int randomNumber = Crystal::RandomFloatInRange(0, 8);
	auto& resourceManager = Crystal::ResourceManager::Instance();


	switch (randomNumber)
	{
	case 0:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_1_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_1_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_1_N.tga");
		staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_1.fbx"));
		break;
	case 1:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_2_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_2_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_2_N.tga");
		staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_2.fbx"));
		break;
	case 2:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_3_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_3_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_3_N.tga");
		staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_3.fbx"));
		break;
	case 3:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_4_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_4_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_4_N.tga");
		staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_4.fbx"));
		break;
	case 4:
	case 5:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_6_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_6_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_6_N.tga");
		staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_6.fbx"));
		break;
	case 6:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_7_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_7_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_7_N.tga");
		staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_7.fbx"));
		break;
	case 7:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_8_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_8_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_8_N.tga");
		staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_8.fbx"));
		break;
	case 8:
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_9_D.tga");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_9_R.tga");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Asteroid2/T_Asteroid_9_N.tga");
		staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/S_Asteroid_9.fbx"));
		break;
	}


	auto randomScale = Crystal::RandomFloatInRange(0.4f, 0.8f) * 0.025f;
	m_MainComponent->RotateRoll(rand() % 360);
	m_MainComponent->RotatePitch(rand() % 360);
	m_MainComponent->RotateYaw(rand() % 360);
	m_MainComponent->SetUnitScale(randomScale);
	m_MainComponent->SetMass(500.0f * randomScale);
}

void DestructibleAsteroid::Initialize()
{
	Actor::Initialize();

	auto barBgMat = Crystal::CreateShared<Crystal::Material>();
	barBgMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/hpBarBg.png");
	barBgMat->bUseAlbedoTextureAlpha = true;

	m_HealthBarBgComponent = CreateComponent<Crystal::TextureComponent>("HpBackgroundTextureComponent");
	m_HealthBarBgComponent->AddMaterial(barBgMat);
	m_HealthBarBgComponent->SetScaleX(0.072f);
	m_HealthBarBgComponent->SetScaleY(0.048f);

	auto hpBarFillMat = Crystal::CreateShared<Crystal::Material>();
	hpBarFillMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/hpBarFill.png");
	hpBarFillMat->bUseAlbedoTextureAlpha = true;

	m_HealthBarWidth = hpBarFillMat->AlbedoTexture.lock()->GetWidth();
	m_HealthBarHeight = hpBarFillMat->AlbedoTexture.lock()->GetHeight();

	m_HealthBarFillComponent = CreateComponent<Crystal::TextureComponent>("HpFillTextureComponent");
	m_HealthBarFillComponent->AddMaterial(hpBarFillMat);
	m_HealthBarFillComponent->SetScaleX(0.072f);
	m_HealthBarFillComponent->SetScaleY(0.048f);

	m_AIPerceptionSourceComponent = CreateComponent<Crystal::AIPerceptionSourceComponent>("AIPerceptionSourceComponent");
	m_AIPerceptionSourceComponent->SetIsHearingEnabled(true);
	m_AIPerceptionSourceComponent->SetIsSightEnabled(true);
	

	SetShowHealthBar(false);
}

void DestructibleAsteroid::SetShowHealthBar(bool bShow)
{
	m_bShouldShowHealthBar = bShow;
	if (m_bShouldShowHealthBar)
	{
		m_HealthBarShowTimer.Reset();
	}

	m_HealthBarBgComponent->SetHiddenInGame(!bShow);
	m_HealthBarFillComponent->SetHiddenInGame(!bShow);
}

void DestructibleAsteroid::Update(float deltaTime)
{
	Actor::Update(deltaTime);

	if (m_bShouldShowHealthBar)
	{
		auto level = Crystal::Cast<Crystal::Level>(GetLevel());
		if (level)
		{
			float healthPercent = m_CurrentHealth / m_MaxHealth;
			healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);


			auto playerController = Crystal::Cast<Crystal::PlayerController>(level->GetPlayerController(0));
			if (playerController)
			{
				auto position2D = playerController->ProjectWorldToCameraSpace(GetPosition());
				position2D.y += 100.0f;
				m_HealthBarBgComponent->SetWorldPosition({position2D.x, position2D.y, 2.0f});
				m_HealthBarFillComponent->SetWorldPosition({
					position2D.x - m_HealthBarWidth * m_HealthBarBgComponent->GetScale().x * (1.0f - healthPercent), position2D.y, 1.0f
				});
			}
		}
	}


	if (m_bShouldShowHealthBar)
	{
		m_HealthBarShowTimer.Tick();
		if (m_HealthBarShowTimer.GetElapsedTime() >= m_HealthBarShowTime)
		{
			m_HealthBarShowTimer.Reset();
			SetShowHealthBar(false);
		}
	}
}

void DestructibleAsteroid::UpdateHealth()
{
	float healthPercent = m_CurrentHealth / m_MaxHealth;
	healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);
	m_HealthBarFillComponent->SetScaleX(m_HealthBarBgComponent->GetScale().x * healthPercent);
}

void DestructibleAsteroid::OnTakeDamage(float damage, Crystal::Weak<Actor> causer)
{
	auto damageCauser = causer.lock();
	if (!damageCauser)
	{
		return;
	}


	auto staticType = damageCauser->StaticType();
	if (staticType != "MyPlayerPawn" && staticType != "AllyDrone")
	{
		return;
	}

	SetShowHealthBar(true);
}

void HealAsteroid::Initialize()
{
	DestructibleAsteroid::Initialize();

	auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
	boundingSphereComponent->SetRadius(45.0f);

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


	auto randomScale = rand() % 6 + 5;

	m_MainComponent->RotateRoll(rand() % 360);
	m_MainComponent->RotatePitch(rand() % 360);
	m_MainComponent->RotateYaw(rand() % 360);
	m_MainComponent->SetUnitScale(randomScale);
	m_MainComponent->SetMass(5000.0f * randomScale);
}

void HealAsteroid::OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser)
{
	DestructibleAsteroid::OnTakeDamage(damage, damageCauser);


	auto staticType = damageCauser.lock()->StaticType();
	if (staticType == "MyPlayerPawn" || staticType == "AllyDrone")
	{
		m_CurrentHealth -= damage;
		UpdateHealth();
		if (m_CurrentHealth <= 0.0f)
		{
			Destroy();
			auto level = Crystal::Cast<Crystal::Level>(GetLevel());
			if (level)
			{
				auto playerPawn = Crystal::Cast<MyPlayerPawn>(level->GetPlayerPawn());
				playerPawn->OnItemDestroyed(ItemType_Heal);
			}
		}
	}
}

void PowerAsteroid::Initialize()
{
	DestructibleAsteroid::Initialize();

	auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
	boundingSphereComponent->SetRadius(45.0f);

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


	auto randomScale = rand() % 6 + 5;

	m_MainComponent->RotateRoll(rand() % 360);
	m_MainComponent->RotatePitch(rand() % 360);
	m_MainComponent->RotateYaw(rand() % 360);
	m_MainComponent->SetUnitScale(randomScale);
	m_MainComponent->SetMass(5000.0f * randomScale);
}

void PowerAsteroid::OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser)
{
	DestructibleAsteroid::OnTakeDamage(damage, damageCauser);

	auto staticType = damageCauser.lock()->StaticType();
	if (staticType == "MyPlayerPawn" || staticType == "AllyDrone")
	{
		m_CurrentHealth -= damage;
		UpdateHealth();
		if (m_CurrentHealth <= 0.0f)
		{
			Destroy();
			auto level = Crystal::Cast<Crystal::Level>(GetLevel());
			if (level)
			{
				auto playerPawn = Crystal::Cast<MyPlayerPawn>(level->GetPlayerPawn());
				playerPawn->OnItemDestroyed(ItemType_Power);
			}
		}
	}
}

void ShieldAsteroid::Initialize()
{
	DestructibleAsteroid::Initialize();

	auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
	boundingSphereComponent->SetRadius(45.0f);

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


	auto randomScale = rand() % 6 + 5;

	m_MainComponent->RotateRoll(rand() % 360);
	m_MainComponent->RotatePitch(rand() % 360);
	m_MainComponent->RotateYaw(rand() % 360);
	m_MainComponent->SetUnitScale(randomScale);
	m_MainComponent->SetMass(5000.0f * randomScale);
}

void ShieldAsteroid::OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser)
{
	DestructibleAsteroid::OnTakeDamage(damage, damageCauser);

	auto staticType = damageCauser.lock()->StaticType();
	if (staticType == "MyPlayerPawn" || staticType == "AllyDrone")
	{
		m_CurrentHealth -= damage;
		UpdateHealth();
		if (m_CurrentHealth <= 0.0f)
		{
			Destroy();
			auto level = Crystal::Cast<Crystal::Level>(GetLevel());
			if (level)
			{
				auto playerPawn = Crystal::Cast<MyPlayerPawn>(level->GetPlayerPawn());
				playerPawn->OnItemDestroyed(ItemType_Shield);
			}
		}
	}
}
