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

		
		Shared<Shader> get(const std::string& fileName)
		{
			auto it = m_Shaders.find(fileName);

			Shared<Shader> returnValue = nullptr;

			if (it == m_Shaders.end() || it->second.expired())
			{
				returnValue = CreateShared<Shader>(fileName);
				m_Shaders[fileName] = returnValue;
			}
			else
			{
				returnValue = m_Shaders[fileName].lock();
			}

			return returnValue;
		}


	private:
		std::unordered_map<std::string, Weak<Shader>> m_Shaders;
	};

}
