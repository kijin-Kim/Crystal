#pragma once
#include <dxgi1_2.h>
#include "Crystal/GamePlay/Components/PrimitiveComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/Resources/Shader.h"


namespace Crystal {
	class LightComponent;
	class DescriptorAllocator;
	class WindowsWindow;
	class Pipeline;
	class LightPipeline;
	class CommandQueue;
	class Texture;

	class RenderSystem : public Actor
	{		
	public:
		RenderSystem();
		~RenderSystem() override = default;

		void Begin() override;

		void LoadEngineContents();
		

		
		void Update(const float deltaTime) override;

		
		void ChangeResolution(int width, int height);
		void ChangeDisplayMode();

		void CreateRenderTargets();
		void DestroyRenderTargets();
		void CreateDepthStencilView();
		


		void RegisterLightComponent(std::weak_ptr<LightComponent> componentWeak);
		void RegisterPrimitiveComponentNew(std::weak_ptr<PrimitiveComponent> componentWeak);


		
		

		template <class T>
		std::unique_ptr<T> CreatePipeline(std::weak_ptr<Shader> shader, const std::string& name)
		{
			/*Pipeline을 만듭니다.*/
			auto pipeline = std::make_unique<T>();
			pipeline->SetOuter(shader);
			pipeline->SetObjectName(name);
			pipeline->OnCreate();

			return pipeline;
		}




		STATIC_TYPE_IMPLE(RenderSystem)
	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain1> m_SwapChain = nullptr;


		
		std::weak_ptr<Texture> m_ColorBufferTextures[2];
		std::weak_ptr<Texture> m_DepthStencilBufferTexture;
		std::weak_ptr<Texture> m_BrightColorBuffer;
		std::weak_ptr<Texture> m_FloatingPointBuffer;

		//=== G-Buffers ==
		std::weak_ptr<Texture> m_AlbedoBuffer;
		std::weak_ptr<Texture> m_RoughnessMetallicAoBuffer;
		std::weak_ptr<Texture> m_EmissiveBuffer;
		std::weak_ptr<Texture> m_WorldNormalBuffer;
		std::weak_ptr<Texture> m_IrradianceBuffer;
		std::weak_ptr<Texture> m_WorldPositionBuffer;


		// TODO 
		std::shared_ptr<Texture> m_PanoTexture;
		std::shared_ptr<Texture> m_CubemapTexture;
		std::shared_ptr<Texture> m_IrradiancemapTexture;

		std::vector<std::unique_ptr<Pipeline>> m_Pipelines;
		std::vector<std::unique_ptr<LightPipeline>> m_LightPipelines;
		
		uint32_t m_RtvIndex = 0;

		uint32_t m_ResWidth = 1920;
		uint32_t m_ResHeight = 1080;


		bool m_bIsFullScreen = false;


		
	};
}

