#pragma once
#include "Actor.h"
#include "Crystal/GamePlay/Components/MeshComponent.h"
#include "Crystal/GamePlay/Components/InputComponent.h"
#include "Crystal/AssetManager/ShaderManager.h"

namespace Crystal {
	class Pawn : public Actor
	{
	public:
		Pawn()
		{
			////// TEMPORARY ////

			/*�ؽ��ĸ� ����ϴ�.*/
			std::shared_ptr<Texture> albedoTexture = std::make_shared<Texture>("assets/textures/Megaphone/Megaphone_01_16-bit_Diffuse.png");
			std::shared_ptr<Texture> roughnessTexture = std::make_shared<Texture>("assets/textures/Megaphone/Megaphone_01_16-bit_Roughness.png");
			std::shared_ptr<Texture> metalicTexture = std::make_shared<Texture>("assets/textures/Megaphone/Megaphone_01_16-bit_Metallic.png");
			std::shared_ptr<Texture> normalTexture = std::make_shared<Texture>("assets/textures/Megaphone/Megaphone_01_16-bit_Normal.png");
			/*�� �ؽ��� ���ҽ��� ���� Shader Resource View�� ����ϴ�.*/
			albedoTexture->CreateShaderResourceView(albedoTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
			metalicTexture->CreateShaderResourceView(metalicTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
			roughnessTexture->CreateShaderResourceView(roughnessTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
			normalTexture->CreateShaderResourceView(normalTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);


			/*�ؽ��ĵ��� ��� �ϳ��� ���͸���� ����ϴ�.*/
			std::shared_ptr<Material> pbrMaterial = std::make_shared<Material>(ShaderManager::Instance().GetShader("PBRShader"));
			pbrMaterial->Set("AlbedoTexture", albedoTexture);
			pbrMaterial->Set("MetalicTexture", metalicTexture);
			pbrMaterial->Set("RoughnessTexture", roughnessTexture);
			pbrMaterial->Set("NormalTexture", normalTexture);


			/*�޽��� ����� ���͸����� �����մϴ�.*/
			std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>("assets/models/Megaphone_01.fbx");
			mesh->SetMaterial(pbrMaterial);
		

			/*���� �޽��� �޽�������Ʈ�� Set�մϴ�.*/
			m_MeshComponent = new MeshComponent();
			m_MeshComponent->SetMesh(mesh);
			

			m_MainComponent = m_MeshComponent;
			//m_MeshComponent->AttachToComponent(m_MainComponent);
		}

		virtual ~Pawn()
		{
			delete m_MeshComponent;
			delete m_InputComponent;
		}

		virtual void Start() override
		{
			Actor::Start();
		}
		virtual void Update(float deltaTime) override
		{
			Actor::Update(deltaTime); // Updating All Component
		}

		virtual void SetupInputComponent(InputComponent* inputComponent)
		{
		}

		void MoveForward(float value)
		{
			__debugbreak();
		}

	private:
		//////////////////Temporary//////////////
		// Need to be memeber of some specialized child class e.g. MeshActor
		MeshComponent* m_MeshComponent = nullptr;
		InputComponent* m_InputComponent = nullptr;
	};
}
