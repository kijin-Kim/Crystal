#pragma once
#include <d3d12.h>

#include "Crystal/Resources/Shader.h"

namespace Crystal {

	struct RenderTargetDescription
	{
		RenderTargetDescription(D3D12_RT_FORMAT_ARRAY rtFormatArray, DXGI_FORMAT dsvFormat)
		{
			RenderTargetFormatArray = rtFormatArray;
			DSVFormat = dsvFormat;
		}

		RenderTargetDescription(DXGI_FORMAT dsvFormat)
		{
			RenderTargetFormatArray = {};
			DSVFormat = dsvFormat;
		}

		D3D12_RT_FORMAT_ARRAY RenderTargetFormatArray;
		DXGI_FORMAT DSVFormat;
	};

	struct PipelineStateStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopology;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_HS HS;
		CD3DX12_PIPELINE_STATE_STREAM_DS DS;
		CD3DX12_PIPELINE_STATE_STREAM_GS GS;
		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencilState;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
		CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC BlendDesc;
	};

	class StateHelper
	{
	public:
		StateHelper() = default;
		~StateHelper() = default;

		static const D3D12_BLEND_DESC Opaque;
		static const D3D12_BLEND_DESC Premultiplied;
		static const D3D12_BLEND_DESC Additive;
		static const D3D12_BLEND_DESC NonPremultiplied;

		static const D3D12_DEPTH_STENCIL_DESC DepthEnable;
		static const D3D12_DEPTH_STENCIL_DESC DepthEnableNoWrite;
		static const D3D12_DEPTH_STENCIL_DESC DepthDisable;
		
		
		static const D3D12_RASTERIZER_DESC CullNone;
		static const D3D12_RASTERIZER_DESC CullCounterClock;
	};

	struct PipelineStateDescription
	{
		PipelineStateDescription(const D3D12_INPUT_LAYOUT_DESC& inputLayoutDesc, const D3D12_BLEND_DESC& blendDesc,
		                         const D3D12_DEPTH_STENCIL_DESC& depthStencil,
		                         const D3D12_RASTERIZER_DESC& rasterizerDesc, RenderTargetDescription renderTargetDescription,
		                         D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopology);
		~PipelineStateDescription() = default;


		void CreatePipelineState(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature, Shared<Shader> shader,
		                         Microsoft::WRL::ComPtr<ID3D12PipelineState>& outPipelineState);

		PipelineStateStream m_PipelineStateStream = {};
	};


}
