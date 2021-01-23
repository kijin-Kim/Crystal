#include "cspch.h"
#include "CommandQueue.h"
#include "Renderer.h"

namespace Crystal {
	CommandQueue::CommandQueue(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type) :
		m_d3d12CommandListType(type)
	{
		D3D12_COMMAND_QUEUE_DESC d3d12CommandQueueDesc = {};
		d3d12CommandQueueDesc.Type = type;
		d3d12CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		d3d12CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		d3d12CommandQueueDesc.NodeMask = 0;

		HRESULT hr = device->CreateCommandQueue(&d3d12CommandQueueDesc, IID_PPV_ARGS(&m_d3d12CommandQueue));
		CS_FATAL(SUCCEEDED(hr), "CommandQueue를 생성하는데 실패하였습니다.");

		hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_d3d12Fence));
		CS_FATAL(SUCCEEDED(hr), "Fence를 생성하는데 실패하였습니다.");

		m_FenceEvent = ::CreateEvent(NULL, false, false, NULL);
		CS_FATAL(m_FenceEvent, "Fence Event를 생성하는데 실패하였습니다");
	}

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> CommandQueue::GetCommandList()
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> d3d12CommandAllcator = nullptr;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> d3d12CommandList = nullptr;
		
		if (!m_CommandAllocatorQueue.empty() && IsExecutionCompleted(m_CommandAllocatorQueue.front().FenceValue))
		{
			d3d12CommandAllcator = m_CommandAllocatorQueue.front().CommandAlloctor;
			d3d12CommandAllcator->Reset();
			m_CommandAllocatorQueue.pop();
		}
		else
		{
			auto device = Renderer::Instance().GetDevice();
			HRESULT hr = device->CreateCommandAllocator(m_d3d12CommandListType, IID_PPV_ARGS(&d3d12CommandAllcator));
			CS_FATAL(SUCCEEDED(hr), "CommandAllocator를 생성하는데 실패하였습니다.");
			d3d12CommandAllcator->SetName(L"Command Allocator");
		}

		if (!m_CommandListQueue.empty())
		{
			d3d12CommandList = m_CommandListQueue.front();
			d3d12CommandList->Reset(d3d12CommandAllcator.Get(), nullptr);
			m_CommandListQueue.pop();
		}
		else
		{
			auto device = Renderer::Instance().GetDevice();
			HRESULT hr = device->CreateCommandList(0, m_d3d12CommandListType, d3d12CommandAllcator.Get(), nullptr, IID_PPV_ARGS(&d3d12CommandList));
			CS_FATAL(SUCCEEDED(hr), "CommandList를 생성하는데 실패하였습니다.");
			d3d12CommandList->SetName(L"Command List");
		}

		d3d12CommandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), d3d12CommandAllcator.Get());

		return d3d12CommandList;
	}

	uint64_t CommandQueue::Execute(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		commandList->Close();
		ID3D12CommandList* const commandLists[] = { commandList.Get() };
		m_d3d12CommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
		uint64_t fenceValue = SignalFence();

		ID3D12CommandAllocator* d3d12CommandAllocator = nullptr;
		UINT dataSize = sizeof(d3d12CommandAllocator);
		commandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &d3d12CommandAllocator);

		m_CommandAllocatorQueue.emplace(CommandAlloctorEntry{ d3d12CommandAllocator, fenceValue });
		d3d12CommandAllocator->Release();

		m_CommandListQueue.push(commandList);

		return fenceValue;
	}

	bool CommandQueue::IsExecutionCompleted(uint64_t fenceValue)
	{
		return m_d3d12Fence->GetCompletedValue() >= fenceValue;
	}

	uint64_t CommandQueue::SignalFence()
	{
		const uint64_t fenceValue = ++m_FenceValue;
		m_d3d12CommandQueue->Signal(m_d3d12Fence.Get(), fenceValue);
		return fenceValue;
	}

	void CommandQueue::Flush()
	{
		WaitForGpuExecutionComplete(SignalFence());
	}

	void CommandQueue::WaitForGpuExecutionComplete(uint64_t fenceValue)
	{
		if (!IsExecutionCompleted(fenceValue))
		{
			m_d3d12Fence->SetEventOnCompletion(fenceValue, m_FenceEvent);
			::WaitForSingleObject(m_FenceEvent, INFINITE);
		}
	}
}