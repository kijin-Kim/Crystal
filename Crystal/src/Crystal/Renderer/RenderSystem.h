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
		RenderSystem() = default;
		~RenderSystem() override = default;

		void Begin() override;

		

		
		void Update(const float deltaTime) override;

		
		void ChangeResolution(int width, int height);
		void ChangeDisplayMode();

		void CreateRenderTargets();
		void DestroyRenderTargets();
		void CreateDepthStencilView();
		


		void RegisterLightComponent(Weak<LightComponent> componentWeak);
		void RegisterPrimitiveComponentNew(Weak<PrimitiveComponent> componentWeak);


		
		

		template <class T>
		Unique<T> CreatePipeline(Weak<Shader> shader, const std::string& name)
		{
			/*Pipeline을 만듭니다.*/
			auto pipeline = std::make_unique<T>();
			pipeline->SetOuter(shader);
			pipeline->SetObjectName(name);
			pipeline->OnCreate();

			return pipeline;
		}

		template <class T>
		Unique<T> NewCreatePipeline(Shared<Shader> shader, const std::string& name)
		{
			/*Pipeline을 만듭니다.*/
			auto pipeline = std::make_unique<T>();
			pipeline->SetShader(shader);
			pipeline->SetOuter(weak_from_this());
			pipeline->SetObjectName(name);
			pipeline->OnCreate();

			return pipeline;
		}




		STATIC_TYPE_IMPLE(RenderSystem)
	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain1> m_SwapChain = nullptr;


		
		//Weak<Texture> m_ColorBufferTextures[2];
		//Weak<Texture> m_DepthStencilBufferTexture;
		//Weak<Texture> m_BrightColorBuffer;
		//Weak<Texture> m_FloatingPointBuffer;



		// TODO 
		/*Shared<Texture> m_PanoTexture;
		Shared<Texture> m_CubemapTexture;
		Shared<Texture> m_IrradiancemapTexture;*/

		std::vector<Unique<Pipeline>> m_Pipelines;
		std::vector<Unique<LightPipeline>> m_LightPipelines;
		
		uint32_t m_RtvIndex = 0;

		uint32_t m_ResWidth = 1920;
		uint32_t m_ResHeight = 1080;


		bool m_bIsFullScreen = false;


		
	};
}

