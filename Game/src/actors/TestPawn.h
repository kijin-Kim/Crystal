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
		std::shared_ptr<Crystal::Texture> roughnessTexture = std::make_shared<Crystal::Texture>("assets/textures/castle_brick_02_red_1k_png/castle_brick_02_red_rough_1k.png");
		std::shared_ptr<Crystal::Texture> metalicTexture = std::make_shared<Crystal::Texture>("assets/textures/22-rp_manuel_animated_001_dancing/rp_manuel_animated_001_mask01.jpg");
		std::shared_ptr<Crystal::Texture> normalTexture = std::make_shared<Crystal::Texture>("assets/textures/castle_brick_02_red_1k_png/castle_brick_02_red_nor_1k.png");

		/*�� �ؽ��� ���ҽ��� ���� Shader Resource View�� ����ϴ�.*/
		albedoTexture->CreateShaderResourceView(albedoTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		metalicTexture->CreateShaderResourceView(metalicTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		roughnessTexture->CreateShaderResourceView(roughnessTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		normalTexture->CreateShaderResourceView(normalTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);


		/*�ؽ��ĵ��� ��� �ϳ��� ���͸���� ����ϴ�.*/
		auto& shaderManager = Crystal::ShaderManager::Instance();
		std::shared_ptr<Crystal::Material> pbrMaterial = std::make_shared<Crystal::Material>(shaderManager.GetShader("PBRShader"));

		pbrMaterial->Set("AlbedoTexture", albedoTexture);
		pbrMaterial->Set("MetalicTexture", metalicTexture);
		pbrMaterial->Set("RoughnessTexture", roughnessTexture);
		pbrMaterial->Set("NormalTexture", normalTexture);


		/*�޽��� ����� ���͸����� �����մϴ�.*/
		//std::shared_ptr<Crystal::Mesh> mesh = std::make_shared<Crystal::Mesh>("assets/models/Sphere.fbx");
		std::shared_ptr<Crystal::Mesh> mesh = std::make_shared<Crystal::Mesh>("assets/models/silly_dancing.fbx");
		mesh->SetMaterial(pbrMaterial);


		/*���� �޽��� �޽�������Ʈ�� Set�մϴ�.*/
		m_MeshComponent = std::make_shared<Crystal::MeshComponent>();
		m_MeshComponent->SetMesh(mesh);


		m_MainComponent = m_MeshComponent;
	}

	virtual ~TestPawn()
	{
	}

	virtual void Start() override
	{
		Pawn::Start();
	}
	virtual void Update(float deltaTime) override
	{
		Pawn::Update(deltaTime); // Updating All Component
	}

	virtual void SetupInputComponent(Crystal::InputComponent* inputComponent) override
	{
	}

};
