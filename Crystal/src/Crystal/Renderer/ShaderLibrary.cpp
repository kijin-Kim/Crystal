#include "cspch.h"
#include "ShaderLibrary.h"

namespace Crystal {

	bool ShaderLibrary::s_bIsInstantiated = false;

	ShaderLibrary::ShaderLibrary()
	{
		CS_ASSERT(!s_bIsInstantiated, "���̴� ���̺귯���� �̹� �����մϴ�");
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