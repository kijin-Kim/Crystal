#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "Shader.h"

namespace Crystal {
	class ShaderManager
	{
		friend class ResourceManager;
	private:
		std::weak_ptr<Shader> createFromFile(const std::string& fileName, const std::string& shaderName);
		void destroy(const std::string& shaderName);

		std::weak_ptr<Shader> get(const std::string& fileName)
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
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
	};
}
