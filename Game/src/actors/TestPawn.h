#pragma once
#include "Crystal/GamePlay/Actors/Pawn.h"

class TestPawn : public Crystal::Pawn
{
public:
	TestPawn()
	{
		////// TEMPORARY ////


		/*�ؽ��ĸ� ����ϴ�.*/
		std::shared_ptr<Crystal::Texture> albedoTexture = std::make_shared<Crystal::Texture>("assets/textures/Stormtrooper/Stormtrooper_D.png");
		/*std::shared_ptr<Crystal::Texture> roughnessTexture = std::make_shared<Crystal::Texture>("assets/textures/castle_brick_02_red_1k_png/castle_brick_02_red_rough_1k.png");
		std::shared_ptr<Crystal::Texture> metalicTexture = std::make_shared<Crystal::Texture>("assets/textures/22-rp_manuel_animated_001_dancing/rp_manuel_animated_001_mask01.jpg");
		std::shared_ptr<Crystal::Texture> normalTexture = std::make_shared<Crystal::Texture>("assets/textures/castle_brick_02_red_1k_png/castle_brick_02_red_nor_1k.png");*/

		/*�� �ؽ��� ���ҽ��� ���� Shader Resource View�� ����ϴ�.*/
		albedoTexture->CreateShaderResourceView(albedoTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		/*metalicTexture->CreateShaderResourceView(metalicTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		roughnessTexture->CreateShaderResourceView(roughnessTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		normalTexture->CreateShaderResourceView(normalTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);*/


		/*Material�� �����մϴ�.*/
		auto& shaderManager = Crystal::ShaderManager::Instance();
		std::shared_ptr<Crystal::Material> pbrMaterial = std::make_shared<Crystal::Material>(shaderManager.GetShader("PBRShader"));

		pbrMaterial->Set("AlbedoTexture", albedoTexture);
		pbrMaterial->Set("RoughnessConstant", 0.3f);
		pbrMaterial->Set("MetalicConstant", 1.0f);

		
		/*pbrMaterial->Set("MetalicTexture", metalicTexture);
		pbrMaterial->Set("RoughnessTexture", roughnessTexture);
		pbrMaterial->Set("NormalTexture", normalTexture);*/


		/*�޽��� ����� Material�� �����մϴ�.*/
		//std::shared_ptr<Crystal::Mesh> mesh = std::make_shared<Crystal::Mesh>("assets/models/silly_dancing.fbx");
		Crystal::Mesh* mesh = new Crystal::Mesh("assets/models/silly_dancing.fbx");
		mesh->SetMaterial(pbrMaterial);

		//std::shared_ptr<Crystal::Mesh> mesh2 = std::make_shared<Crystal::Mesh>("assets/models/silly_dancing.fbx");
		Crystal::Mesh* mesh2 = new Crystal::Mesh("assets/models/silly_dancing.fbx");
		mesh2->SetMaterial(pbrMaterial);

		


		/*���� �޽��� �޽�������Ʈ�� Set�մϴ�.*/
		m_MeshComponent = CreateComponent<Crystal::MeshComponent>("MeshComponent");
		m_MeshComponent->SetMesh(mesh);

		m_TempFollowMeshComponent = CreateComponent<Crystal::MeshComponent>("FollowTempMeshComponent");
		m_TempFollowMeshComponent->SetMesh(mesh2);

		/* TEMP */

		m_TempFollowMeshComponent->SetLocalTransform(Crystal::Matrix4x4::Translation({ 0.0f, 2.0f, 0.0f }));

		m_MainComponent = m_MeshComponent;

		m_TempFollowMeshComponent->AttachToComponent(m_MainComponent);
		
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
	Crystal::MeshComponent* m_TempFollowMeshComponent = nullptr;
};
