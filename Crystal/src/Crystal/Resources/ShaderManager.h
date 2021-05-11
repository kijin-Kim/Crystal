#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "Shader.h"
#include "Crystal/Core/Core.h"

namespace Crystal {
	class ShaderManager
	{
		friend class ResourceManager;
	private:
		Weak<Shader> createFromFile(const std::string& fileName, const std::string& shaderName);
		void destroy(const std::string& shaderName);

		Weak<Shader> get(const std::string& fileName)
		{
			auto it = m_Shaders.find(fileName);
			if (it == m_Shaders.end())
				CS_FATAL(false, "Shader : %s를 찾을 수 없습니다.", fileName.c_str());
			return m_Shaders[fileName];
		}

	private:
		ShaderManager() = default;
		~ShaderManager() = default;

	private:
		std::unordered_map<std::string, Shared<Shader>> m_Shaders;
	};


	class NewShaderManager
	{
	public:
		NewShaderManager() = default;
		~NewShaderManager() = default;

		
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
