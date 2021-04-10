#pragma once
#include <memory>
#include <d3d12.h>
#include <map>
#include <queue>
#include <wrl/client.h>
#include "DirectXTex/d3dx12.h"

// This is based on tutorial from https://www.3dgep.com/learning-directx-12-3/

/*
 *  Copyright(c) 2018 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

namespace Crystal {

	class DescriptorAllocation;

	class DescriptorAllocatorPage : public std::enable_shared_from_this<DescriptorAllocatorPage>
	{
	public:
		DescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t descriptorCount);

		D3D12_DESCRIPTOR_HEAP_TYPE GetDescriptorHeapType() const { return m_DescriptorHeapType; }

		bool HasSpace(uint32_t descriptorCount) // descriptor count 만큼의 space가 있는지 체크합니다.
		{
			return m_FreeListBySize.lower_bound(descriptorCount) != m_FreeListBySize.end();
		}

		uint32_t GetFreeHandleCount() const { return m_FreeHandleCount; }

		/*
		* 현재 descriptor heap에 descriptorCount만큼의 descriptor를 할당
		* descriptorCount만큼 할당을 할 수 없으면 null DescriptorAllocation을 리턴
		* Free Handle Count보다 descriptorCount가 작아도 단편화로 인해 실패 할 수 있음.
		*/
		DescriptorAllocation Allocate(uint32_t descriptorCount);

		/*
		* Descriptor를 heap에 다시 돌려줍니다.
		* Stale Allocation은 직접적으로 해제되지 않고 다른 컨테이너로 옮겨지고 해제 됩니다.
		* r-value reference로 DescriptorAllocation을 받기 때문에 Free 함수 이후에
		* DescriptorAllocation은 empty가 됩니다.
		*/
		void Free(DescriptorAllocation&& descriptor);

		void ReleaseStaleDescriptors();

	protected:
		uint32_t ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle);

		void AddNewBlock(uint32_t offset, uint32_t descriptorCount);

		/*
		* Block을 Free합니다.
		* Free된 Block은 Free list에서 merge되어 큰 Block을 만듭니다.
		*/
		void FreeBlock(uint32_t offset, uint32_t descriptorCount);

	private:
		using OffsetType = uint32_t;
		using SizeType = uint32_t;

		struct FreeBlockInfo;

		// Bimap
		using FreeListByOffset = std::map<OffsetType, FreeBlockInfo>;
		using FreeListBySize = std::multimap<SizeType, FreeListByOffset::iterator>;

		/*
		* FreeListByOffset의 오브젝트.
		*/
		struct FreeBlockInfo
		{
			FreeBlockInfo(SizeType size) : Size(size)
			{
			}

			SizeType Size;
			FreeListBySize::iterator FreeListBySizeIt; // Merging하기 위해 
		};

		struct StaleDescriptorInfo
		{
			StaleDescriptorInfo(OffsetType offset, SizeType size)
				: Offset(offset),
				Size(size)
			{
			}


			OffsetType Offset;
			SizeType Size;
		};

		using StaleDescriptorQueue = std::queue<StaleDescriptorInfo>;

		FreeListByOffset m_FreeListByOffset;
		FreeListBySize m_FreeListBySize;
		StaleDescriptorQueue m_StaleDescriptors;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_D3d12DescriptorHeap;
		D3D12_DESCRIPTOR_HEAP_TYPE m_DescriptorHeapType;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_BaseDescriptor;

		uint32_t m_DescriptorHandleIncrementSize;
		uint32_t m_DescriptorCountInHeap;
		uint32_t m_FreeHandleCount;


	};

}