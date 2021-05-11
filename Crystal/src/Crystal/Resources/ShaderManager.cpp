#include "cspch.h"
#include "ShaderManager.h"

namespace Crystal {
	std::weak_ptr<Shader> ShaderManager::createFromFile(const std::string& fileName, const std::string& shaderName)
	{
		if (m_Shaders.find(shaderName) != m_Shaders.end())
		{
			CS_FATAL(false, "%s이 이미 존재합니다", shaderName);
			return {};
		}

		auto newShader = std::make_shared<Shader>(fileName);
		newShader->SetObjectName(shaderName);
		newShader->OnCreate();
		return (m_Shaders[shaderName] = std::move(newShader));
	}

	void ShaderManager::destroy(const std::string& shaderName)
	{
		auto it = m_Shaders.find(shaderName);
		if (it == m_Shaders.end())
			CS_FATAL(false, "Shader : %s를 찾을 수 없습니다.", shaderName.c_str());
		m_Shaders.erase(shaderName);
	}

}