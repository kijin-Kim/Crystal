#include "cspch.h"
#include "CommandList.h"
#include "Renderer.h"

namespace Crystal {
	CommandList::CommandList()
	{
		auto device = Renderer::Instance().GetDevice();
		HRESULT hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator));
		CS_FATAL(SUCCEEDED(hr), "Command Allocator �� �����ϴµ� �����Ͽ����ϴ�");
		hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_CommandList));
		CS_FATAL(SUCCEEDED(hr), "Command List�� �����ϴµ� �����Ͽ����ϴ�");
	}

	CommandList::CommandList(ID3D12PipelineState* initialPipelinState)
	{
		auto device = Renderer::Instance().GetDevice();
		HRESULT hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator));
		CS_FATAL(SUCCEEDED(hr), "Command Allocator �� �����ϴµ� �����Ͽ����ϴ�");
		hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), initialPipelinState, IID_PPV_ARGS(&m_CommandList));
		CS_FATAL(SUCCEEDED(hr), "Command List�� �����ϴµ� �����Ͽ����ϴ�");
	}
}