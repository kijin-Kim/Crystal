#pragma once
#include <unordered_map>
#include "Descriptor.h"
#include "Crystal/Core/Logger.h"

namespace Crystal {

	class Texture final : public DescriptorObject
	{
	public:
		/*�������� �Ӽ����� �ް� ���� ���ҽ��� ����ϴ�.*/
		Texture(int width, int height, int depth, int mipLevels, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS resourceFlags, D3D12_RESOURCE_STATES initialStates);
		/*���Ϸκ��� �ؽ��ĸ� �ε��մϴ�.*/
		Texture(const std::string& filePath, D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE);
		/*���ҽ��� �ǳ׹޾� �並 ����ϴ�.*/
		Texture(ID3D12Resource* resource, D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE);
		virtual ~Texture() = default;

		void CreateShaderResourceView(DXGI_FORMAT format, D3D12_SRV_DIMENSION srvDimension);
		void CreateUnorderedAccessView() {}
		void CreateRenderTargetView(DXGI_FORMAT format, D3D12_RTV_DIMENSION rtvDimension);
		void CreateDepthStencilView(DXGI_FORMAT format, D3D12_DSV_DIMENSION dsvDimension);

		D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const
		{
			if (m_ShaderResourceView.IsNull())
				CS_ASSERT(false, "Shader Resource View�� ���� �������ּ���");
			return m_ShaderResourceView.GetDescriptorHandle();
		}
		D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const
		{
			if (m_RenderTargetView.IsNull())
				CS_ASSERT(false, "Render Target View�� ���� �������ּ���");
			return m_RenderTargetView.GetDescriptorHandle();
		}


		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const
		{
			if (m_DepthStencilView.IsNull())
				CS_ASSERT(false, "Depth Stencil View�� ���� �������ּ���");
			return m_DepthStencilView.GetDescriptorHandle();
		}

		ID3D12Resource* GetResource() { return m_Resource.Get(); }

	public:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource = nullptr;
		Descriptor m_ShaderResourceView = {};
		Descriptor m_UnorderedAccessView = {};
		Descriptor m_DepthStencilView = {};
		Descriptor m_RenderTargetView = {};
	};




}
