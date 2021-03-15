#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "Shader.h"

namespace Crystal {
	class ShaderManager final
	{
	public:
		static ShaderManager& Instance() { static ShaderManager instance; return instance; }

		void Load(const std::string& fileName, const std::string& shaderName);
		void Add(const std::shared_ptr<Shader>& shader, const std::string& shaderName);

		std::shared_ptr<Shader> GetShader(const std::string& fileName) 
		{ 
			auto it = m_Shaders.find(fileName);
			if (it == m_Shaders.end())
				CS_FATAL(false, "Shader : %s�� ã�� �� �����ϴ�.", fileName.c_str());
			return m_Shaders[fileName]; 
		}

	private:
		ShaderManager() = default;
		~ShaderManager() = default;

	private:
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
	};
}