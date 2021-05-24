#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "Shader.h"
#include "Crystal/Core/Core.h"

namespace Crystal {

	class ShaderManager
	{
	public:
		ShaderManager() = default;
		~ShaderManager() = default;

		
		Weak<Shader> get(const std::string& fileName)
		{
			auto it = m_Shaders.find(fileName);

			if (it != m_Shaders.end())
			{
				return it->second;
			}
		
			m_Shaders[fileName] = CreateShared<Shader>(fileName);
			return m_Shaders[fileName];
		}


	private:
		std::unordered_map<std::string, Shared<Shader>> m_Shaders;
	};

}
