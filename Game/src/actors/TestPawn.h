#pragma once
#include "Crystal/GamePlay/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Resources/Animation.h"

class TestPawn : public Crystal::Pawn
{
public:
	TestPawn()
	{
		////// TEMPORARY ////


		/*텍스쳐를 만듭니다.*/
		std::shared_ptr<Crystal::Texture> albedoTexture = std::make_shared<Crystal::Texture>("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Dif.tga");
		std::shared_ptr<Crystal::Texture> normalTexture = std::make_shared<Crystal::Texture>("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Norm.tga");


		/*std::shared_ptr<Crystal::Texture> albedoTexture = std::make_shared<Crystal::Texture>("assets/textures/Megaphone/Megaphone_01_16-bit_Diffuse.png");
		std::shared_ptr<Crystal::Texture> normalTexture = std::make_shared<Crystal::Texture>("assets/textures/Megaphone/Megaphone_01_16-bit_Normal.png");*/

		/*각 텍스쳐 리소스에 대한 Shader Resource View를 만듭니다.*/
		albedoTexture->CreateShaderResourceView(albedoTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		//metalicTexture->CreateShaderResourceView(metalicTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		//roughnessTexture->CreateShaderResourceView(roughnessTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		normalTexture->CreateShaderResourceView(normalTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);


		/*Material을 생성합니다.*/
		auto& shaderManager = Crystal::ShaderManager::Instance();
		std::shared_ptr<Crystal::Material> pbrMaterial = std::make_shared<Crystal::Material>(shaderManager.GetShader("PBRShader_Skeletal"));

		pbrMaterial->Set("AlbedoTexture", albedoTexture);
		pbrMaterial->Set("NormalTexture", normalTexture);

		pbrMaterial->Set("RoughnessConstant", 0.3f);
		pbrMaterial->Set("MetalicConstant", 0.5f);

		
		/*pbrMaterial->Set("MetalicTexture", metalicTexture);
		pbrMaterial->Set("RoughnessTexture", roughnessTexture);
		pbrMaterial->Set("NormalTexture", normalTexture);*/
		

		/*Crystal::SkeletalMesh* mesh = new Crystal::SkeletalMesh("assets/models/silly_dancing.fbx");
		mesh->SetMaterial(pbrMaterial);
		m_MeshComponent = CreateComponent<Crystal::SkeletalMeshComponent>("MeshComponent");
		m_MeshComponent->SetRenderable(mesh);*/

		Crystal::StaticMesh* mesh = new Crystal::StaticMesh("assets/models/SM_Frigate_BE2.fbx");
		//Crystal::StaticMesh* mesh = new Crystal::StaticMesh("assets/models/Megaphone_01.fbx");
		mesh->SetMaterial(pbrMaterial);
		m_MeshComponent = CreateComponent<Crystal::StaticMeshComponent>("MeshComponent");
		m_MeshComponent->SetRenderable(mesh);

		m_MainComponent = m_MeshComponent;





	
		/*Crystal::SkeletalMesh* skeletalMesh = new Crystal::SkeletalMesh("assets/models/HeroTPP.fbx", "assets/models/Run_Rt.FBX");
		skeletalMesh->SetMaterial(pbrMaterial);
		m_TempFollowMeshComponent = CreateComponent<Crystal::SkeletalMeshComponent>("FollowTempMeshComponent");
		m_TempFollowMeshComponent->SetRenderable(skeletalMesh);
		m_TempFollowMeshComponent->SetLocalTransform(Crystal::Matrix4x4::Translation({ 0.0f, 2.0f, 0.0f }));
		m_TempFollowMeshComponent->AttachToComponent(m_MainComponent);*/
		
	}

	virtual ~TestPawn()
	{
	}

	virtual void Begin() override
	{
		Pawn::Begin();
		CS_DEBUG_INFO("Test Pawn Begin");
	}

	virtual void End() override
	{
		Pawn::End();
		CS_DEBUG_INFO("Test Pawn End");
	}

	virtual void Update(float deltaTime) override
	{
		Pawn::Update(deltaTime); // Updating All Component
	}

	virtual void SetupInputComponent(Crystal::InputComponent* inputComponent) override
	{
	}
};
