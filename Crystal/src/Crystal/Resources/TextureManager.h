#pragma once
#include <unordered_map>
#include <memory>
#include "Texture.h"

namespace Crystal {
	class TextureManager
	{
		friend class ResourceManager;
	private:
		std::weak_ptr<Texture> createFromFile(const std::string& filePath, const std::string& alias = "")
		{
			std::string name;
			name = alias.empty() ? filePath : alias;

			if (m_Textures.find(name) != m_Textures.end())
			{
				CS_FATAL(false, "%s�� �̹� �����մϴ�", alias);
				return std::weak_ptr<Texture>(); // Unreachable
			}
			return (m_Textures[name] = std::make_shared<Texture>(filePath));
		}

		std::weak_ptr<Texture> create(int width, int height, int depth, int mipLevels, DXGI_FORMAT format,
			D3D12_RESOURCE_FLAGS resourceFlags, D3D12_RESOURCE_STATES initialStates, const std::string& name)
		{
			if (m_Textures.find(name) != m_Textures.end())
			{
				CS_FATAL(false, "%s�� �̹� �����մϴ�", name);
				return std::weak_ptr<Texture>(); // Unreachable
			}

			return (m_Textures[name] = std::make_shared<Texture>(width, height, depth,
				mipLevels, format, resourceFlags, initialStates));
		}

		void destroy(const std::string& name)
		{
			auto it = m_Textures.find(name);
			if (it == m_Textures.end())
				CS_FATAL(false, "Texture : %s�� ã�� �� �����ϴ�.", name.c_str());
			m_Textures.erase(name);
		}

		std::weak_ptr<Texture> get(const std::string& name)
		{
			auto it = m_Textures.find(name);
			if (it == m_Textures.end())
				CS_FATAL(false, "Texture : %s�� ã�� �� �����ϴ�.", name.c_str());
			return m_Textures[name];
		}

	private:
		TextureManager() = default;
		~TextureManager() = default;

	private:
		std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
	};
}
