#pragma once
#include "DescriptorAllocation.h"
#include "Crystal/Core/Core.h"
#include <map>

namespace Crystal {

	
	class Buffer
	{
	public:
		Buffer(void* data, UINT sizeInByte, UINT count, bool bAlign256, bool bAsDynamic);
		~Buffer() {}

		void SetData(void* data, uint32_t offset, uint32_t size);
		

		D3D12_CPU_DESCRIPTOR_HANDLE AsConstantBufferView();
		D3D12_VERTEX_BUFFER_VIEW AsVertexBufferView(UINT stride) const;
		D3D12_INDEX_BUFFER_VIEW AsIndexBufferView() const;

		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

		void CreateConstantBufferView();


		uint32_t GetElementCount() const;
		uint32_t GetSize() const;

		void SetElementCount(uint32_t count) { m_ElementCount = count; }

		bool GetIsDynamic() const { return m_bIsDynamic; }
		bool GetIsAligned256() const { return m_bIsAligned256; }

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource = nullptr;
		DescriptorAllocation m_ConstantBufferView = {};
		UINT8* m_CpuBasePtr = nullptr;

		uint32_t m_Size;
		uint32_t m_ElementCount;

		bool m_bIsDynamic = false;
		bool m_bIsAligned256 = false;
	};

	class BufferManager
	{
	public:
		static BufferManager& Instance()
		{
			static BufferManager instance;
			return instance;
		}
		
		BufferManager() = default;
		~BufferManager() = default;
		BufferManager(const BufferManager& other) = delete;
		BufferManager(BufferManager&& other) noexcept = delete;
		BufferManager& operator=(const BufferManager& other) = delete;
		BufferManager& operator=(BufferManager&& other) noexcept = delete;

		
		Shared<Buffer> GetConstantBuffer(void* data, UINT requestedSize);
		Shared<Buffer> GetBuffer(void* data, UINT requestedSize, UINT count, bool bAsDynamic);

		void Flush();

	private:
		std::multimap<UINT, Shared<Buffer>> m_AvailableConstantBuffers;
		std::vector<Shared<Buffer>> m_UsedConstantBuffers;

		struct BufferKey
		{
			UINT Size = 0;
			bool bIsDynmaic = false;

			bool operator==(const BufferKey& other) const
			{
				return Size == other.Size && bIsDynmaic == other.bIsDynmaic;
			}
			
			bool operator<(const BufferKey& other) const
			{
				if(Size == other.Size)
				{
					return Size < other.Size;
				}

				return bIsDynmaic < other.bIsDynmaic;
			}
		};
		
		std::multimap<BufferKey, Shared<Buffer>> m_AvailableBuffers;
		std::vector<Shared<Buffer>> m_UsedBuffers;

		
	};
}


