#include "cspch.h"
#include "PipelineStateHelper.h"

#include "Crystal/Core/Device.h"
#include "Crystal/Renderer/CommandQueue.h"

namespace Crystal {

	const D3D12_BLEND_DESC StateHelper::Opaque =
	{
		FALSE,
		FALSE,
		{
			{
				FALSE,
				FALSE,
				D3D12_BLEND_ONE,
				D3D12_BLEND_ZERO,
				D3D12_BLEND_OP_ADD,
				D3D12_BLEND_ONE,
				D3D12_BLEND_ZERO,
				D3D12_BLEND_OP_ADD,
				D3D12_LOGIC_OP_NOOP,
				D3D12_COLOR_WRITE_ENABLE_ALL
			}
		}
	};


	const D3D12_BLEND_DESC StateHelper::Premultiplied =
	{
		FALSE,
		FALSE,
		{
			{
				TRUE,
				FALSE,
				D3D12_BLEND_ONE,
				D3D12_BLEND_INV_SRC_ALPHA,
				D3D12_BLEND_OP_ADD,
				D3D12_BLEND_ONE,
				D3D12_BLEND_INV_SRC_ALPHA,
				D3D12_BLEND_OP_ADD,
				D3D12_LOGIC_OP_NOOP,
				D3D12_COLOR_WRITE_ENABLE_ALL
			}
		}
	};

	const D3D12_BLEND_DESC StateHelper::Additive =
	{
		FALSE,
		FALSE,
		{
			{
				TRUE,
				FALSE,
				D3D12_BLEND_SRC_ALPHA,
				D3D12_BLEND_ONE,
				D3D12_BLEND_OP_ADD,
				D3D12_BLEND_SRC_ALPHA,
				D3D12_BLEND_ONE,
				D3D12_BLEND_OP_ADD,
				D3D12_LOGIC_OP_NOOP,
				D3D12_COLOR_WRITE_ENABLE_ALL
			}
		}
	};


	const D3D12_BLEND_DESC StateHelper::NonPremultiplied =
	{
		FALSE,
		FALSE,
		{
			{
				TRUE,
				FALSE,
				D3D12_BLEND_SRC_ALPHA,
				D3D12_BLEND_INV_SRC_ALPHA,
				D3D12_BLEND_OP_ADD,
				D3D12_BLEND_SRC_ALPHA,
				D3D12_BLEND_INV_SRC_ALPHA,
				D3D12_BLEND_OP_ADD,
				D3D12_LOGIC_OP_NOOP,
				D3D12_COLOR_WRITE_ENABLE_ALL
			}
		}
	};


	const D3D12_DEPTH_STENCIL_DESC StateHelper::DepthEnable =
	{
		TRUE,
		D3D12_DEPTH_WRITE_MASK_ALL,
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		FALSE,
		D3D12_DEFAULT_STENCIL_READ_MASK,
		D3D12_DEFAULT_STENCIL_WRITE_MASK,
		{
			D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP_KEEP,
			D3D12_COMPARISON_FUNC_ALWAYS
		},
		{
			D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP_KEEP,
			D3D12_COMPARISON_FUNC_ALWAYS
		}
	};

	const D3D12_DEPTH_STENCIL_DESC StateHelper::DepthEnableNoWrite =
	{
		TRUE,
		D3D12_DEPTH_WRITE_MASK_ZERO,
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		FALSE,
		D3D12_DEFAULT_STENCIL_READ_MASK,
		D3D12_DEFAULT_STENCIL_WRITE_MASK,
		{
			D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP_KEEP,
			D3D12_COMPARISON_FUNC_ALWAYS
		},
		{
			D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP_KEEP,
			D3D12_COMPARISON_FUNC_ALWAYS
		}
	};

	const D3D12_DEPTH_STENCIL_DESC StateHelper::DepthDisable =
	{
		FALSE,
		D3D12_DEPTH_WRITE_MASK_ZERO,
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		FALSE,
		D3D12_DEFAULT_STENCIL_READ_MASK,
		D3D12_DEFAULT_STENCIL_WRITE_MASK,
		{
			D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP_KEEP,
			D3D12_COMPARISON_FUNC_ALWAYS
		},
		{
			D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP_KEEP,
			D3D12_COMPARISON_FUNC_ALWAYS
		}
	};


	const D3D12_RASTERIZER_DESC StateHelper::CullNone =
	{
		D3D12_FILL_MODE_SOLID,
		D3D12_CULL_MODE_NONE,
		FALSE,
		D3D12_DEFAULT_DEPTH_BIAS,
		D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
		TRUE,
		TRUE,
		FALSE,
		0,
		D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};

	const D3D12_RASTERIZER_DESC StateHelper::CullCounterClock =
	{
		D3D12_FILL_MODE_SOLID,
		D3D12_CULL_MODE_BACK,
		FALSE,
		D3D12_DEFAULT_DEPTH_BIAS,
		D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
		TRUE,
		TRUE,
		FALSE,
		0,
		D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};


	//============================================================================================================

	PipelineStateDescription::PipelineStateDescription(const D3D12_INPUT_LAYOUT_DESC& inputLayoutDesc, const D3D12_BLEND_DESC& blendDesc,
	                                                   const D3D12_DEPTH_STENCIL_DESC& depthStencil, const D3D12_RASTERIZER_DESC& rasterizerDesc,
	                                                   RenderTargetDescription renderTargetDescription,
	                                                   D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopology)
	{
		m_PipelineStateStream.InputLayout = inputLayoutDesc;
		m_PipelineStateStream.BlendDesc = CD3DX12_BLEND_DESC(blendDesc);
		m_PipelineStateStream.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(depthStencil);
		m_PipelineStateStream.Rasterizer = CD3DX12_RASTERIZER_DESC(rasterizerDesc);
		m_PipelineStateStream.RTVFormats = renderTargetDescription.RenderTargetFormatArray;
		m_PipelineStateStream.DSVFormat = renderTargetDescription.DSVFormat;
		m_PipelineStateStream.PrimitiveTopology = primitiveTopology;
	}

	void PipelineStateDescription::CreatePipelineState(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature, Shared<Shader> shader,
	                                                   Microsoft::WRL::ComPtr<ID3D12PipelineState>& outPipelineState)
	{
		auto d3dDevice = Device::Instance().GetD3DDevice();


		auto& shaderDatablobs = shader->GetRaw();

		m_PipelineStateStream.RootSignature = rootSignature.Get();

		if (shader->HasShader(ShaderType::Vertex))
		{
			m_PipelineStateStream.VS =
			{
				shaderDatablobs[ShaderType::Vertex]->GetBufferPointer(),
				shaderDatablobs[ShaderType::Vertex]->GetBufferSize()
			};
		}

		if (shader->HasShader(ShaderType::Hull))
		{
			m_PipelineStateStream.HS =
			{
				shaderDatablobs[ShaderType::Hull]->GetBufferPointer(),
				shaderDatablobs[ShaderType::Hull]->GetBufferSize()
			};
		}

		if (shader->HasShader(ShaderType::Domain))
		{
			m_PipelineStateStream.DS =
			{
				shaderDatablobs[ShaderType::Domain]->GetBufferPointer(),
				shaderDatablobs[ShaderType::Domain]->GetBufferSize()
			};
		}

		if (shader->HasShader(ShaderType::Geometry))
		{
			m_PipelineStateStream.GS =
			{
				shaderDatablobs[ShaderType::Geometry]->GetBufferPointer(),
				shaderDatablobs[ShaderType::Geometry]->GetBufferSize()
			};
		}

		if (shader->HasShader(ShaderType::Pixel))
		{
			m_PipelineStateStream.PS =
			{
				shaderDatablobs[ShaderType::Pixel]->GetBufferPointer(),
				shaderDatablobs[ShaderType::Pixel]->GetBufferSize()
			};
		}


		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {sizeof(m_PipelineStateStream), &m_PipelineStateStream};

		HRESULT hr = d3dDevice->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&outPipelineState));
		CS_FATAL(SUCCEEDED(hr), "Pipeline State 를 생성하는데 실패하였습니다");
	}


}
