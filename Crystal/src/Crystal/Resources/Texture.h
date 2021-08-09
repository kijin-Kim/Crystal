#pragma once
#include <unordered_map>
#include <boost/container_hash/hash.hpp>

#include "Crystal/Core/Logger.h"
#include "DescriptorAllocation.h"

namespace Crystal {


	struct DescriptorKey
	{
		
		
		int Dimension = 0;
		DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN;

		friend bool operator==(const DescriptorKey& lhs, const DescriptorKey& rhs)
		{
			return lhs.Dimension == rhs.Dimension
				&& lhs.Format == rhs.Format;
		}

		friend bool operator!=(const DescriptorKey& lhs, const DescriptorKey& rhs)
		{
			return !(lhs == rhs);
		}
	};

	struct DescriptorKeyHasher
	{
		std::size_t operator()(const DescriptorKey& k) const
		{
			using boost::hash_value;
			using boost::hash_combine;

			std::size_t seed = 0;
			hash_combine(seed, hash_value(k.Dimension));
			hash_combine(seed, hash_value(k.Format));
			return seed;
		}
	};

	
	
	class Texture
	{
	public:
		/*여러가지 속성들을 받고 직접 리소스를 만듭니다.*/
		Texture(int width, int height, int depth, int mipLevels, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS resourceFlags,
		        D3D12_RESOURCE_STATES initialStates);
		/*파일로부터 텍스쳐를 로드합니다.*/
		Texture(const std::string& filePath, D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE);
		/*리소스를 건네받아 뷰를 만듭니다.*/
		Texture(ID3D12Resource* resource, D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE);
		virtual ~Texture() = default;

		DescriptorAllocation CreateShaderResourceView(D3D12_SRV_DIMENSION srvDimension, DXGI_FORMAT format);
		DescriptorAllocation CreateUnorderedAccessView(D3D12_UAV_DIMENSION uavDimension, DXGI_FORMAT format);
		DescriptorAllocation CreateRenderTargetView(D3D12_RTV_DIMENSION rtvDimension, DXGI_FORMAT format);
		DescriptorAllocation CreateDepthStencilView(D3D12_DSV_DIMENSION dsvDimension, DXGI_FORMAT format);


		D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(D3D12_SRV_DIMENSION srvDimension, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);
		D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(D3D12_UAV_DIMENSION uavDimension, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);
		D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView(D3D12_RTV_DIMENSION rtvDimension, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView(D3D12_DSV_DIMENSION dsvDimension, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);


		ID3D12Resource* GetResource() const { return m_Resource.Get(); }

		UINT64 GetWidth() const { return m_Resource->GetDesc().Width; }
		UINT64 GetHeight() const { return m_Resource->GetDesc().Height; }

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource = nullptr;

		std::unordered_map<DescriptorKey, DescriptorAllocation, DescriptorKeyHasher> m_ShaderResourceViews;
		std::unordered_map<DescriptorKey, DescriptorAllocation, DescriptorKeyHasher> m_UnorderedAccessViews;
		std::unordered_map<DescriptorKey, DescriptorAllocation, DescriptorKeyHasher> m_RenderTargetViews;
		std::unordered_map<DescriptorKey, DescriptorAllocation, DescriptorKeyHasher> m_DepthStencilViews;
		
	};
}
