#include "cspch.h"
#include "ShaderManager.h"

namespace Crystal {
	void ShaderManager::createFromFile(const std::string& fileName, const std::string& shaderName)
	{
		if (m_Shaders.find(shaderName) != m_Shaders.end())
		{
			CS_FATAL(false, "%s�� �̹� �����մϴ�", shaderName);
			return;
		}

		m_Shaders[shaderName] = std::make_shared<Shader>(fileName);
	}

	void ShaderManager::destroy(const std::string& shaderName)
	{
		auto it = m_Shaders.find(shaderName);
		if (it == m_Shaders.end())
			CS_FATAL(false, "Shader : %s�� ã�� �� �����ϴ�.", shaderName.c_str());
		m_Shaders.erase(shaderName);
	}
}