#include "cspch.h"
#include "ShaderLibrary.h"

namespace Crystal {

	bool ShaderLibrary::s_bIsInstantiated = false;

	ShaderLibrary::ShaderLibrary()
	{
		CS_ASSERT(!s_bIsInstantiated, "셰이더 라이브러리가 이미 존재합니다");
		s_bIsInstantiated = true;
	}

	ShaderLibrary::~ShaderLibrary()
	{
		s_bIsInstantiated = false;
	}

	void ShaderLibrary::Load(const std::string& fileName, const std::string& shaderName)
	{
		m_Shaders[shaderName] = std::make_shared<Shader>(fileName);
	}

	void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader, const std::string& shaderName)
	{
		m_Shaders[shaderName] = shader;
	}

}