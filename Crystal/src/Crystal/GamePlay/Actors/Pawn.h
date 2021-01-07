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

			/*텍스쳐를 만듭니다.*/
			std::shared_ptr<Texture> albedoTexture = std::make_shared<Texture>("assets/textures/Megaphone/Megaphone_01_16-bit_Diffuse.png");
			std::shared_ptr<Texture> roughnessTexture = std::make_shared<Texture>("assets/textures/Megaphone/Megaphone_01_16-bit_Roughness.png");
			std::shared_ptr<Texture> metalicTexture = std::make_shared<Texture>("assets/textures/Megaphone/Megaphone_01_16-bit_Metallic.png");
			std::shared_ptr<Texture> normalTexture = std::make_shared<Texture>("assets/textures/Megaphone/Megaphone_01_16-bit_Normal.png");
			/*각 텍스쳐 리소스에 대한 Shader Resource View를 만듭니다.*/
			albedoTexture->CreateShaderResourceView(albedoTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
			metalicTexture->CreateShaderResourceView(metalicTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
			roughnessTexture->CreateShaderResourceView(roughnessTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
			normalTexture->CreateShaderResourceView(normalTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);


			/*텍스쳐들을 모아 하나의 머터리얼로 만듭니다.*/
			std::shared_ptr<Material> pbrMaterial = std::make_shared<Material>(ShaderManager::Instance().GetShader("PBRShader"));
			pbrMaterial->Set("AlbedoTexture", albedoTexture);
			pbrMaterial->Set("MetalicTexture", metalicTexture);
			pbrMaterial->Set("RoughnessTexture", roughnessTexture);
			pbrMaterial->Set("NormalTexture", normalTexture);


			/*메쉬를 만들고 머터리얼을 지정합니다.*/
			std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>("assets/models/Megaphone_01.fbx");
			mesh->SetMaterial(pbrMaterial);
		

			/*최종 메쉬를 메쉬컴포넌트에 Set합니다.*/
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
