#include "cspch.h"
#include "TonemappingPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Pipelines/PipelineStateHelper.h"
#include "Crystal/Resources/ResourceManager.h"
#include "Crystal/Renderer/Scene.h"

namespace Crystal {


	void TonemappingPipeline::OnCreate()
	{
		auto device = Device::Instance().GetD3DDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 500;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV_SRV���� �����ϴµ� �����Ͽ����ϴ�.");


		auto inputLayout = m_Shader->GetInputLayout();


		D3D12_RT_FORMAT_ARRAY rtvFormat = {};
		rtvFormat.NumRenderTargets = 1;
		rtvFormat.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;


		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {inputLayout.GetData(), inputLayout.GetCount()};
		RenderTargetDescription renderTargetDescription(rtvFormat, DXGI_FORMAT_D24_UNORM_S8_UINT);

		PipelineStateDescription pipelineStateDescription(
			inputLayoutDesc,
			StateHelper::Opaque,
			StateHelper::DepthDisable,
			StateHelper::CullCounterClock,
			renderTargetDescription,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
		);


		pipelineStateDescription.CreatePipelineState(m_Shader->GetRootSignature().GetData(), m_Shader, m_PipelineState);

	}

	void TonemappingPipeline::Begin(const Shared<Scene>& scene)
	{
		RenderPipeline::Begin(scene);


		auto device = Device::Instance().GetD3DDevice();

		auto heapHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();


		device->CopyDescriptorsSimple(1, heapHandle,
		                              scene->FloatingPointBuffer->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void TonemappingPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		auto device = Device::Instance().GetD3DDevice();
		auto rootSignature = m_Shader->GetRootSignature();


		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetGraphicsRootSignature(rootSignature.GetData());
		ID3D12DescriptorHeap* staticDescriptorHeaps[] = { m_DescriptorHeap.Get() };
		commandList->SetDescriptorHeaps(_countof(staticDescriptorHeaps), staticDescriptorHeaps);

		D3D12_GPU_DESCRIPTOR_HANDLE descriptorHeapHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		commandList->SetGraphicsRootDescriptorTable(0, descriptorHeapHandle);


		auto renderable = GetScene()->PlaneQuad2DMesh;

		for (int j = 0; j < renderable->GetVertexbufferCount(); j++)
		{
			renderable->Render(commandList, j);
		}
	}


}


