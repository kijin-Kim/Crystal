#include "cspch.h"
#include "FontPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/Renderer/Scene.h"


namespace Crystal {


	void FontPipeline::OnCreate()
	{
		auto d3d12Device = Device::Instance().GetD3DDevice();

		m_GraphicsMemory = std::make_unique<DirectX::GraphicsMemory>(d3d12Device);

		m_ResourceDescriptors = std::make_unique<DirectX::DescriptorHeap>(d3d12Device, 1);

		DirectX::ResourceUploadBatch resourceUploadBatch(d3d12Device);
		resourceUploadBatch.Begin();

		m_Font = std::make_unique<DirectX::SpriteFont>(d3d12Device, resourceUploadBatch,
		                                               L"assets/fonts/myfile.spritefont",
		                                               m_ResourceDescriptors->GetCpuHandle(0),
		                                               m_ResourceDescriptors->GetGpuHandle(0));

		auto commandQueue = Device::Instance().GetCommandQueue();


		DirectX::RenderTargetState rtState(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D24_UNORM_S8_UINT);
		DirectX::SpriteBatchPipelineStateDescription pd(rtState);

		m_SpriteBatch = std::make_unique<DirectX::SpriteBatch>(d3d12Device, resourceUploadBatch, pd);

		auto uploadResourcesFinished = resourceUploadBatch.End(commandQueue->GetRaw());
		uploadResourcesFinished.wait();
	}


	void FontPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		auto& scene = GetScene();
		auto mainCamera = scene->Cameras[0].lock();
		if (!mainCamera)
		{
			return;
		}


		m_SpriteBatch->SetViewport(mainCamera->GetViewport());
		ID3D12DescriptorHeap* heaps[] = {m_ResourceDescriptors->Heap()};
		commandList->SetDescriptorHeaps(static_cast<UINT>(std::size(heaps)), heaps);

		m_SpriteBatch->Begin(commandList.Get());

		for (auto& weak : scene->Texts)
		{
			auto text = weak.lock();
			if (!text)
			{
				continue;
			}


			auto output = text->GetText();
			if(output.empty())
			{
				continue;
			}

			auto horizontalAlignment = text->GetHorizontalTextAlignment();
			DirectX::XMFLOAT2 origin = { 0.0f, 0.0f };

			switch (horizontalAlignment)
			{
			case TextAlignment::TA_Default: 
				break;
			case TextAlignment::TA_Center:
				DirectX::XMFLOAT2 stringMeasure = { 0.0f, 0.0f };
				DirectX::XMStoreFloat2(&stringMeasure, m_Font->MeasureString(output.c_str()));
				origin.x = stringMeasure.x / 2.0f;
				break;
			default: ;
			}

			auto verticalAlignment = text->GetVerticalTextAlignment();
			switch (verticalAlignment)
			{
			case TextAlignment::TA_Default:
				break;
			case TextAlignment::TA_Center:
				DirectX::XMFLOAT2 stringMeasure = { 0.0f, 0.0f };
				DirectX::XMStoreFloat2(&stringMeasure, m_Font->MeasureString(output.c_str()));
				origin.y = stringMeasure.y / 2.0f;
				break;
			default:;
			}
			
			m_Font->DrawString(m_SpriteBatch.get(), output.c_str(), {text->GetWorldPosition().x, mainCamera->GetViewport().Height - text->GetWorldPosition().y}, DirectX::XMLoadFloat3(&text->GetTextColor()), 0.0f, origin,
			                   1 / 16.0f * (float)text->GetSize(), DirectX::SpriteEffects_None, text->GetWorldPosition().z);
		}


		m_SpriteBatch->End();
	}
}
