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
		//std::shared_ptr<Crystal::Texture> albedoTexture = std::make_shared<Crystal::Texture>("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Dif.tga");
		std::shared_ptr<Crystal::Texture> albedoTexture = std::make_shared<Crystal::Texture>("assets/textures/T_Frigate_BE2/T_M_SM_Frigate_BE2_MI_Frigate_BE2_White_BaseColor.tga");
		std::shared_ptr<Crystal::Texture> metallicTexture = std::make_shared<Crystal::Texture>("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Metallic.tga");
		std::shared_ptr<Crystal::Texture> roughnessTexture = std::make_shared<Crystal::Texture>("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Roughness.tga");
		std::shared_ptr<Crystal::Texture> normalTexture = std::make_shared<Crystal::Texture>("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Norm.tga");
		
		albedoTexture->CreateShaderResourceView(albedoTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		metallicTexture->CreateShaderResourceView(metallicTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		roughnessTexture->CreateShaderResourceView(roughnessTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		normalTexture->CreateShaderResourceView(normalTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);


		/*Material을 생성합니다.*/
		auto& shaderManager = Crystal::ShaderManager::Instance();
		std::shared_ptr<Crystal::Material> pbrMaterial = std::make_shared<Crystal::Material>(shaderManager.GetShader("PBRShader_Static"));

		pbrMaterial->Set("AlbedoTexture", albedoTexture);
		pbrMaterial->Set("MetallicTexture", metallicTexture);
		pbrMaterial->Set("RoughnessTexture", roughnessTexture);
		pbrMaterial->Set("NormalTexture", normalTexture);

		

		/*Crystal::SkeletalMesh* mesh = new Crystal::SkeletalMesh("assets/models/silly_dancing.fbx");
		mesh->SetMaterial(pbrMaterial);
		m_MeshComponent = CreateComponent<Crystal::SkeletalMeshComponent>("MeshComponent");
		m_MeshComponent->SetRenderable(mesh);*/

		Crystal::StaticMesh* mesh = new Crystal::StaticMesh("assets/models/SM_Frigate_BE2.fbx");
		
		mesh->SetMaterial(pbrMaterial, 0);
		m_MeshComponent = CreateComponent<Crystal::StaticMeshComponent>("MeshComponent");
		m_MeshComponent->SetRenderable(mesh);

		m_MainComponent = m_MeshComponent;
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
private:
	Crystal::StaticMeshComponent* megaphoneMeshComponent = nullptr;
};
