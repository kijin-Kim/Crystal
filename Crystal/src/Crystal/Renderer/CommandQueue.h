#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <queue>

namespace Crystal {
	
	class CommandQueue final
	{
	public:
		CommandQueue(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type);
		~CommandQueue() = default;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> GetCommandList();

		uint64_t Execute(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList);

		bool IsExecutionCompleted(uint64_t fenceValue);

		uint64_t SignalFence();

		void Flush();

		void WaitForGpuExecutionComplete(uint64_t fenceValue);

		ID3D12CommandQueue* GetRaw() const { return m_d3d12CommandQueue.Get(); }

	private:
		struct CommandAlloctorEntry
		{
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAlloctor;
			uint64_t FenceValue;
		};

		using CommandAllocatorQueue = std::queue<CommandAlloctorEntry>;
		using CommandListQueue = std::queue<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2>>;

		D3D12_COMMAND_LIST_TYPE m_d3d12CommandListType;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3d12CommandQueue = nullptr;

		CommandListQueue m_CommandListQueue;
		CommandAllocatorQueue m_CommandAllocatorQueue;

		Microsoft::WRL::ComPtr<ID3D12Fence> m_d3d12Fence = nullptr;
		HANDLE m_FenceEvent = nullptr;
		uint64_t m_FenceValue = 0;
	};

}

// this code is Inspired By https://github.com/jpvanoosten/LearningDirectX12/blob/5af740ce2c29846bf36b9814ae748271075bdcb8/Tutorial2/src/CommandQueue.cpp