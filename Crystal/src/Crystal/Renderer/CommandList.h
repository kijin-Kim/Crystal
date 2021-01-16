#pragma once
#include <d3d12.h>
#include <wrl/client.h>

namespace Crystal {
	class CommandList final
	{
	public:
		CommandList();
		CommandList(ID3D12PipelineState* initialPipelineState);

		ID3D12GraphicsCommandList2* GetRaw() const { return m_CommandList.Get(); }

		void Reset() const { m_CommandAllocator->Reset(); m_CommandList->Reset(m_CommandAllocator.Get(), nullptr); }

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator = nullptr;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> m_CommandList = nullptr;
	};
}
