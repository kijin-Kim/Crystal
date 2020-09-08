#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "Shader.h"

namespace Crystal {

	
	class ShaderLibrary
	{
	public:
		ShaderLibrary();
		~ShaderLibrary();

		void Load(const std::string& fileName, const std::string& shaderName);
		void Add(const std::shared_ptr<Shader>& shader, const std::string& shaderName);

		std::shared_ptr<Shader> GetShader(const std::string& fileName) { return m_Shaders[fileName]; }

	private:
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
		static bool s_bIsInstantiated;
	};
}
