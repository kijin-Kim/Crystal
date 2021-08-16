#pragma once
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/Resources/Material.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/ResourceManager.h"

class TitleInterceptorActor : public Crystal::Actor
{
public:
	TitleInterceptorActor() = default;
	~TitleInterceptorActor() override = default;

	void Initialize() override
	{
		Crystal::Actor::Initialize();

		auto& resourceManager = Crystal::ResourceManager::Instance();

		auto baseMat = Crystal::CreateShared<Crystal::Material>();
		baseMat->AlbedoTexture = resourceManager.GetTexture("assets/textures/T_InterceptorBaseV3_BaseColor.tga");
		baseMat->MetallicTexture = resourceManager.GetTexture("assets/textures/T_InterceptorBaseV3_Metalness.tga");
		baseMat->RoughnessTexture = resourceManager.GetTexture("assets/textures/T_InterceptorBaseV3_Roughness.tga");
		baseMat->NormalTexture = resourceManager.GetTexture("assets/textures/T_InterceptorBaseV3_Normal.tga");
		baseMat->ShadingModel = Crystal::EShadingModel::SM_Lit;
		baseMat->BlendMode = Crystal::EBlendMode::BM_Translucent;

		auto partsMat = Crystal::CreateShared<Crystal::Material>();
		partsMat->AlbedoTexture = resourceManager.GetTexture("assets/textures/T_InterceptorPartsV3_BaseColor.tga");
		partsMat->MetallicTexture = resourceManager.GetTexture("assets/textures/T_InterceptorPartsV3_Metalness.tga");
		partsMat->RoughnessTexture = resourceManager.GetTexture("assets/textures/T_InterceptorPartsV3_Roughness.tga");
		partsMat->NormalTexture = resourceManager.GetTexture("assets/textures/T_InterceptorPartsV3_Normal.tga");
		partsMat->ShadingModel = Crystal::EShadingModel::SM_Lit;
		partsMat->BlendMode = Crystal::EBlendMode::BM_Translucent;

		auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
		staticMeshComponent->AddMaterial(baseMat);
		staticMeshComponent->AddMaterial(partsMat);
		staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MK3InterceptorMainLite.fbx"));
		m_MainComponent = staticMeshComponent;
	}

	void Update(float deltaTime) override
	{
		Actor::Update(deltaTime);

		RotateYaw(45.0f * deltaTime);
	}

	STATIC_TYPE_IMPLE(TitleInterceptorActor)
};

