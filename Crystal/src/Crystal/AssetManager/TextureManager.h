#pragma once
#include <unordered_map>

namespace Crystal {

	class TextureManager final
	{
	public:
		// TODO : Fix it to use pool as ring buffer
		struct Texture
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> Buffer = nullptr;
			D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle;
		};

		static TextureManager& Get() { static TextureManager instance; return instance; }
		
		D3D12_CPU_DESCRIPTOR_HANDLE GetTexture(const std::string& textureName);

		void Load(const std::string& filepath, const std::string& textureName = "");

	private:
		TextureManager();

		~TextureManager() = default;
		TextureManager(const TextureManager&) = delete;
	private:
		std::unordered_map<std::string, Texture> m_TexturePool;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_TexturesHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_TexturePoolCpuHandle;

		UINT m_TexturePoolSize = 1000;
	};
}


