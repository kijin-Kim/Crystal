#include "cspch.h"
#include "ShaderManager.h"

namespace Crystal {

	void ShaderManager::Load(const std::string& fileName, const std::string& shaderName)
	{
		if (m_Shaders.find(shaderName) != m_Shaders.end())
		{
			CS_ASSERT(false, "%s이 이미 존재합니다", shaderName);
			return;
		}

		m_Shaders[shaderName] = std::make_shared<Shader>(fileName);
	}

	void ShaderManager::Add(const std::shared_ptr<Shader>& shader, const std::string& shaderName)
	{
		if (m_Shaders.find(shaderName) != m_Shaders.end())
		{
			CS_ASSERT(false, "%s이 이미 존재합니다", shaderName);
			return;
		}
		m_Shaders[shaderName] = shader;
	}

}