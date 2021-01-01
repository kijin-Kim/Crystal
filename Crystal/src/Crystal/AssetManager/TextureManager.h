#pragma once
#include <unordered_map>

namespace Crystal {
	// Data that API Expose to user (handle of data)
	struct Texture
	{
		D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle;
		UINT Count;
	};

	struct TextureData
	{
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> Buffers;
		Texture texture = {};
	};

	class TextureManager final
	{
	public:
		// TODO : Fix it to use pool as ring buffer
		static TextureManager& Instance() { static TextureManager instance; return instance; }

		Texture GetTexture(const std::string& textureAlias) const;

		void Load(int depth, const std::vector<std::string>& filepaths, const std::string& textureAlias = "");

	private:
		TextureManager();

		~TextureManager() = default;
		TextureManager(const TextureManager&) = delete;
	private:
		mutable std::unordered_map<std::string, TextureData> m_TexturePool;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_TexturesHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_TexturePoolCpuHandle;

		UINT m_TexturePoolSize = 1000;
	};
}
