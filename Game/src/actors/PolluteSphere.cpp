#include "PolluteSphere.h"

#include "MyPlayerPawn.h"

void PolluteSphere::Initialize()
{
	Crystal::Actor::Initialize();

	auto& resourceManager = Crystal::ResourceManager::Instance();

	auto material = Crystal::CreateShared<Crystal::Material>();
	material->EmissiveColor = { 195.0f / 255.0f * 0.5f, 195.0f / 255.0f * 0.5f, 1.0f / 255.0f * 0.5f };
	material->Opacity = 0.1f;
		
	material->ShadingModel = Crystal::EShadingModel::SM_Unlit;
	material->BlendMode = Crystal::EBlendMode::BM_Translucent;
	material->bTwoSided = false;


	auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
	boundingSphereComponent->SetCollisionType(Crystal::ECollisionType::CT_Overlap);
	boundingSphereComponent->SetRadius(2300.0f);
	boundingSphereComponent->IgnoreActorClassOf("Kraken");
	boundingSphereComponent->IgnoreActorClassOf("DroneLaser");
	boundingSphereComponent->SetInverseMass(0.0f);

	boundingSphereComponent->BindOnBeginOverlapEvent([this](const Crystal::OverlapResult& overlapResult)
		{
			auto actor = overlapResult.OverlappedActor.lock();
			auto staticType = actor->StaticType();
			if (staticType == "MyPlayerPawn")
			{
				Crystal::Cast<MyPlayerPawn>(actor)->SetIsPolluteDamagable(false);
				auto& mat = m_StaticMeshComponent->GetMaterials();
				mat[0]->bTwoSided = true;
			}
		});
	boundingSphereComponent->BindOnEndOverlapEvent([this](const Crystal::OverlapResult& overlapResult)
		{
			auto actor = overlapResult.OverlappedActor.lock();
			auto staticType = actor->StaticType();
			if (staticType == "MyPlayerPawn")
			{
				Crystal::Cast<MyPlayerPawn>(actor)->SetIsPolluteDamagable(true);
				auto& mat= m_StaticMeshComponent->GetMaterials();
				mat[0]->bTwoSided = false;
			}
		});


	m_MainComponent = boundingSphereComponent;


	m_StaticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
	m_StaticMeshComponent->AddMaterial(material);
	m_StaticMeshComponent->SetUnitScale(2300.0f);
	m_StaticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/1M_Sphere.fbx"));
	m_StaticMeshComponent->AttachTo(m_MainComponent);

	m_MainComponent = m_StaticMeshComponent;
}
