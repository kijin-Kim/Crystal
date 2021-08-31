#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "DirectXTK/Inc/DescriptorHeap.h"
#include "DirectXTK/Inc/GraphicsMemory.h"
#include "DirectXTK/Inc/ResourceUploadBatch.h"
#include "DirectXTK/Inc/SpriteBatch.h"
#include "DirectXTK/Inc/SpriteFont.h"

namespace Crystal {

	class FontPipeline : public RenderPipeline
	{
	public:


	public:
		FontPipeline() = default;
		~FontPipeline() override = default;

		void OnCreate() override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;

		STATIC_TYPE_IMPLE(FontPipeline)

	private:
		//=====
		std::unique_ptr<DirectX::DescriptorHeap> m_ResourceDescriptors;
		std::unique_ptr<DirectX::SpriteFont> m_Font;

		std::unique_ptr<DirectX::SpriteBatch> m_SpriteBatch;


		std::unique_ptr<DirectX::GraphicsMemory> m_GraphicsMemory;


	};

}
