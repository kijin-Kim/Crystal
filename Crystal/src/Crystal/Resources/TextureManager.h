#pragma once
#include <unordered_map>
#include <memory>
#include "Texture.h"

namespace Crystal {
	class TextureManager
	{
		friend class ResourceManager;
		friend class NewResourceManager;
	private:
		std::weak_ptr<Texture> createFromFile(const std::string& filePath, const std::string& alias, D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE)
		{
			if (m_Textures.find(alias) != m_Textures.end())
			{
				CS_FATAL(false, "%s이 이미 존재합니다", alias.c_str());
				return std::weak_ptr<Texture>(); // Unreachable
			}
			return (m_Textures[alias] = std::make_shared<Texture>(filePath, resourceFlags));
		}

		std::weak_ptr<Texture> create(int width, int height, int depth, int mipLevels, DXGI_FORMAT format,
			D3D12_RESOURCE_FLAGS resourceFlags, D3D12_RESOURCE_STATES initialStates, const std::string& name)
		{
			if (m_Textures.find(name) != m_Textures.end())
			{
				CS_FATAL(false, "%s이 이미 존재합니다", name.c_str());
				return std::weak_ptr<Texture>(); // Unreachable
			}

			return (m_Textures[name] = std::make_shared<Texture>(width, height, depth,
				mipLevels, format, resourceFlags, initialStates));
		}

		std::weak_ptr<Texture> createByResource(ID3D12Resource* resource, const std::string& alias, D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE)
		{
			if (m_Textures.find(alias) != m_Textures.end())
			{
				CS_FATAL(false, "%s이 이미 존재합니다", alias.c_str());
				return std::weak_ptr<Texture>(); // Unreachable
			}

			return (m_Textures[alias] = std::make_shared<Texture>(resource, resourceFlags));
		}


		void destroy(const std::string& name)
		{
			auto it = m_Textures.find(name);
			if (it == m_Textures.end())
				CS_FATAL(false, "Texture : %s를 찾을 수 없습니다.", name.c_str());
			m_Textures.erase(name);
		}

		std::weak_ptr<Texture> get(const std::string& name)
		{
			auto it = m_Textures.find(name);
			if (it == m_Textures.end())
				CS_FATAL(false, "Texture : %s를 찾을 수 없습니다.", name.c_str());
			return m_Textures[name];
		}

	private:
		TextureManager() = default;
		~TextureManager() = default;

	private:
		std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
	};










	class NewTextureManager
	{
	public:
		NewTextureManager() = default;
		~NewTextureManager() = default;
		Shared<Texture> get(const std::string& fileName)
		{
			auto it = m_Textures.find(fileName);
			if (it == m_Textures.end() || it->second.expired())
			{
				auto newTexture = CreateShared<Texture>(fileName);
				m_Textures[fileName] = newTexture;
			}

			return m_Textures[fileName].lock();
		}

	

	private:
		std::unordered_map<std::string, Weak<Texture>> m_Textures;
	};
}
