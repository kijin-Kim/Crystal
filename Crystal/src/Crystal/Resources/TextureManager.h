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
		Weak<Texture> get(const std::string& fileName)
		{
			auto it = m_Textures.find(fileName);

			if (it != m_Textures.end())
			{
				it->second;
			}
			
			m_Textures[fileName] = CreateShared<Texture>(fileName);
			return m_Textures[fileName];
		}


	

	private:
		std::unordered_map<std::string, Weak<Texture>> m_Textures;
	};
}
