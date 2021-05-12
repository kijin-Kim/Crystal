#pragma once
#include <unordered_map>
#include <memory>
#include "Texture.h"

namespace Crystal {

	class TextureManager
	{
	public:
		TextureManager() = default;
		~TextureManager() = default;
		Shared<Texture> get(const std::string& fileName)
		{
			auto it = m_Textures.find(fileName);

			Shared<Texture> returnValue = nullptr;

			if (it == m_Textures.end() || it->second.expired())
			{
				returnValue = CreateShared<Texture>(fileName);
				m_Textures[fileName] = returnValue;
			}
			else
			{
				returnValue = m_Textures[fileName].lock();
			}

			return returnValue;
		}

		void RemoveGarbage()
		{
			for (auto it = m_Textures.begin(); it != m_Textures.end();)
			{
				if (it->second.expired())
				{
					it = m_Textures.erase(it);
				}
				else
				{
					++it;
				}

			}
		}

	

	private:
		std::unordered_map<std::string, Weak<Texture>> m_Textures;
	};
}
