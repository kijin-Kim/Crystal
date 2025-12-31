#pragma once
#include <dxgi1_2.h>

#include "Crystal/Core/Timer.h"
#include "Crystal/GamePlay/Components/PrimitiveComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/Resources/Shader.h"

namespace Crystal {
	struct WorldConfig;
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

		void OnCreate() override;
		void Begin() override;

		void LoadCubemapTextures(const Shared<Scene>& scene);

		
		void Update(const float deltaTime) override;

		
		void ChangeResolution(int width, int height);
		void ChangeDisplayMode();

		void CreateRenderTargets();
		void CreateSwapChain();
		void CreateDepthStencilView();
		

		const WorldConfig& GetWorldConfig() const;

		Shared<World> GetWorld() const;

		Shared<Scene> GetScene();
		Shared<Level> GetLevel();

		void ToggleDisplayMode() { m_bIsFullScreen = !m_bIsFullScreen; }
	

		template <class T>
		Unique<T> CreatePipeline(Shared<Shader> shader, const std::string& name)
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
		Microsoft::WRL::ComPtr<IDXGISwapChain1>
		m_SwapChain = nullptr;


		std::vector<Unique<Pipeline>> m_Pipelines;
		std::vector<Unique<RenderPipeline>> m_LightPipelines;
		
		uint32_t m_RtvIndex = 0;

		uint32_t m_ResWidth = 1920;
		uint32_t m_ResHeight = 1080;


		bool m_bIsFullScreen = false;

		Timer m_CleanUpTimer;
		const float m_CleanUpTime = 5.0f;

	};
}



