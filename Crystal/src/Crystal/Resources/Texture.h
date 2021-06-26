#pragma once
#include <unordered_map>
#include "Crystal/Core/Logger.h"
#include "DescriptorAllocation.h"

namespace Crystal {
	class Texture
	{
	public:
		/*�������� �Ӽ����� �ް� ���� ���ҽ��� ����ϴ�.*/
		Texture(int width, int height, int depth, int mipLevels, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS resourceFlags,
		        D3D12_RESOURCE_STATES initialStates);
		/*���Ϸκ��� �ؽ��ĸ� �ε��մϴ�.*/
		Texture(const std::string& filePath, D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE);
		/*���ҽ��� �ǳ׹޾� �並 ����ϴ�.*/
		Texture(ID3D12Resource* resource, D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE);
		virtual ~Texture() = default;

		DescriptorAllocation CreateShaderResourceView(D3D12_SRV_DIMENSION srvDimension);
		DescriptorAllocation CreateUnorderedAccessView(D3D12_UAV_DIMENSION uavDimension);
		DescriptorAllocation CreateRenderTargetView(D3D12_RTV_DIMENSION rtvDimension);
		DescriptorAllocation CreateDepthStencilView(D3D12_DSV_DIMENSION dsvDimension);


		D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(D3D12_SRV_DIMENSION srvDimension);
		D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(D3D12_UAV_DIMENSION uavDimension);
		D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView(D3D12_RTV_DIMENSION rtvDimension);
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView(D3D12_DSV_DIMENSION dsvDimension);


		ID3D12Resource* GetResource() const { return m_Resource.Get(); }

	private:
	
		
		
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource = nullptr;
		
		std::unordered_map<D3D12_SRV_DIMENSION, DescriptorAllocation> m_ShaderResourceViews;
		std::unordered_map<D3D12_UAV_DIMENSION, DescriptorAllocation> m_UnorderedAccessViews;
		std::unordered_map<D3D12_RTV_DIMENSION, DescriptorAllocation> m_RenderTargetViews;
		std::unordered_map<D3D12_DSV_DIMENSION, DescriptorAllocation> m_DepthStencilViews;

		
		
	};
}
