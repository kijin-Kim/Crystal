#include "cspch.h"
#include "DescriptorAllocatorPage.h"

#include "DescriptorAllocation.h"
#include "Crystal/Core/Device.h"
#include "Crystal/Renderer/CommandQueue.h"

namespace Crystal {
	DescriptorAllocatorPage::DescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t descriptorCount)
		: m_DescriptorHeapType(type),
		m_DescriptorCountInHeap(descriptorCount)
	{
		auto device = Device::Instance().GetD3DDevice();

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type = m_DescriptorHeapType;
		heapDesc.NumDescriptors = m_DescriptorCountInHeap;

		HRESULT hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_D3d12DescriptorHeap));
		// CS_FATAL(SUCCEEDED(hr), "Descriptor Heap을 생성하는데 실패하였습니다");

		m_BaseDescriptor = m_D3d12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_DescriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_FreeHandleCount = m_DescriptorCountInHeap;

		AddNewBlock(0, m_FreeHandleCount); // Add block in offset 0
	}

	DescriptorAllocation DescriptorAllocatorPage::Allocate(uint32_t descriptorCount)
	{
		if (descriptorCount > m_FreeHandleCount)
		{
			return DescriptorAllocation(); // Failed Null Allocation
		}

		auto minBlockIt = m_FreeListBySize.lower_bound(descriptorCount); // Find first meet free-list can afford descriptor count
		if (minBlockIt == m_FreeListBySize.end())
		{
			return DescriptorAllocation();  // Failed Null Allocation
		}

		auto blockSize = minBlockIt->first;
		auto offsetIt = minBlockIt->second;
		auto offset = offsetIt->first;

		// Erase
		m_FreeListBySize.erase(minBlockIt);
		m_FreeListByOffset.erase(offsetIt);

		auto newOffset = offset + descriptorCount;
		auto newSize = blockSize - descriptorCount;

		// Re-insert if left size is not zero
		if (newSize > 0)
		{
			AddNewBlock(newOffset, newSize);
		}

		m_FreeHandleCount -= descriptorCount;

		return { CD3DX12_CPU_DESCRIPTOR_HANDLE(m_BaseDescriptor, offset, m_DescriptorHandleIncrementSize),
		descriptorCount, m_DescriptorHandleIncrementSize, shared_from_this() };
	}

	void DescriptorAllocatorPage::Free(DescriptorAllocation&& descriptor)
	{
		auto offset = ComputeOffset(descriptor.GetDescriptorHandle());
		m_StaleDescriptors.emplace(offset, descriptor.GetDescriptorHandleCount());
	}

	void DescriptorAllocatorPage::ReleaseStaleDescriptors()
	{
		while (!m_StaleDescriptors.empty())
		{
			auto& staleDescriptor = m_StaleDescriptors.front();
			auto offset = staleDescriptor.Offset;
			auto descriptorCount = staleDescriptor.Size;

			FreeBlock(offset, descriptorCount);

			m_StaleDescriptors.pop();
		}
	}

	uint32_t DescriptorAllocatorPage::ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		// increment to handle
		return (handle.ptr - m_BaseDescriptor.ptr) / m_DescriptorHandleIncrementSize;
	}

	void DescriptorAllocatorPage::AddNewBlock(uint32_t offset, uint32_t descriptorCount)
	{
		// insert in bimap
		auto offsetIt = m_FreeListByOffset.emplace(offset, descriptorCount);
		auto sizeIt = m_FreeListBySize.emplace(descriptorCount, offsetIt.first);
		offsetIt.first->second.FreeListBySizeIt = sizeIt;
	}

	void DescriptorAllocatorPage::FreeBlock(uint32_t offset, uint32_t descriptorCount)
	{
		auto nextBlockIt = m_FreeListByOffset.upper_bound(offset);
		auto previousBlockIt = nextBlockIt;
		if (previousBlockIt != m_FreeListByOffset.begin())
		{
			--previousBlockIt;
		}
		else
		{
			previousBlockIt = m_FreeListByOffset.end();
		}

		m_FreeHandleCount += descriptorCount;

		if (previousBlockIt != m_FreeListByOffset.end()
			&& offset == previousBlockIt->first + previousBlockIt->second.Size) // Previous block is freed
		{
			offset = previousBlockIt->first;
			descriptorCount += previousBlockIt->second.Size;

			// Remove previous block
			m_FreeListBySize.erase(previousBlockIt->second.FreeListBySizeIt);
			m_FreeListByOffset.erase(previousBlockIt);
		}

		if (nextBlockIt != m_FreeListByOffset.end() &&
			offset + descriptorCount == nextBlockIt->first) // Nex block is freed
		{
			descriptorCount += nextBlockIt->second.Size;

			// Remove next block
			m_FreeListBySize.erase(nextBlockIt->second.FreeListBySizeIt);
			m_FreeListByOffset.erase(nextBlockIt);
		}

		// Add new merged block
		AddNewBlock(offset, descriptorCount);

	}
}



