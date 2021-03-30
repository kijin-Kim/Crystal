#pragma once
#include "ShaderManager.h"
#include "RenderableManager.h"

namespace Crystal {
	class ResourceManager
	{
	public:
		static ResourceManager& Instance() { static ResourceManager instance; return instance; }

		void LoadShader(const std::string& fileName, const std::string& shaderName) 
		{ 
			m_ShaderManager.load(fileName, shaderName); 
		}

		void UnLoadShader(const std::string& shaderName)
		{
			m_ShaderManager.unLoad(shaderName);
		}

		const std::shared_ptr<Shader>& GetShader(const std::string& fileName)
		{
			return m_ShaderManager.getShader(fileName);
		}


		template<class T>
		void LoadRenderableFromFile(const std::string& filePath, const std::string& alias = "")
		{
			m_RenderableManager.loadFromFile<T>(filePath, alias);
		}

		template<class T>
		void LoadRenderable(const std::string& name)
		{
			m_RenderableManager.load<T>(name);
		}

		void UnLoadRenderable(const std::string& name)
		{
			m_RenderableManager.unLoad(name);
		}
		

		const std::shared_ptr<Renderable>& GetRenderable(const std::string& name)
		{
			return m_RenderableManager.getRenderable(name);
		}
		

	private:
		ResourceManager() = default;
		~ResourceManager() = default;

	private:
		ShaderManager m_ShaderManager;
		RenderableManager m_RenderableManager;
	};


}
