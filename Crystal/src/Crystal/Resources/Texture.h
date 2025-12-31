#pragma once
#include <unordered_map>

#include "Crystal/Core/Logger.h"
#include "DescriptorAllocation.h"

namespace Crystal
{
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
        template <class T>
        inline void hash_combine(std::size_t& seed, T const& v) const
        {
            seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

        template <typename T>
        inline size_t hash_value(const T& value) const
        {
            std::hash<T> hash_func;
            return hash_func(value);
        }


        std::size_t operator()(const DescriptorKey& k) const
        {
            std::size_t seed = 0;
            hash_combine(seed, hash_value(k.Dimension));
            hash_combine(seed, hash_value(k.Format));
            return seed;
        }
    };


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


