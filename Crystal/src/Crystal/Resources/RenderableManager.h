#pragma once
#include <unordered_map>
#include <memory>
#include "Renderable.h"

namespace Crystal {
	class RenderableManager
	{
		friend class ResourceManager;
	private:
		template<class T>
		void createFromFile(const std::string& filePath, const std::string& alias = "")
		{
			std::string name;
			name = alias.empty() ? filePath : alias;

			if (m_Meshes.find(name) != m_Meshes.end())
			{
				CS_FATAL(false, "%s�̸��� ���� Renderable�� �̹� �����մϴ�", name.c_str());
			}

			m_Meshes[name] = std::make_shared<T>(filePath);
		}

		template<class T>
		void create(const std::string& name)
		{
			if (m_Meshes.find(name) != m_Meshes.end())
			{
				CS_FATAL(false, "%s�̸��� ���� Renderable�� �̹� �����մϴ�", name.c_str());
			}

			m_Meshes[name] = std::make_shared<T>();
		}

		void destroy(const std::string& name)
		{
			auto it = m_Meshes.find(name);
			if (it == m_Meshes.end())
				CS_FATAL(false, "Mesh: %s�� ã�� �� �����ϴ�.", name.c_str());
			m_Meshes.erase(name);
		}

		const std::shared_ptr<Renderable>& get(const std::string& name)
		{
			auto it = m_Meshes.find(name);
			if (it == m_Meshes.end())
				CS_FATAL(false, "Renderable: %s�� ã�� �� �����ϴ�.", name.c_str());
			return m_Meshes[name];
		}

	private:
		RenderableManager() = default;
		~RenderableManager() = default;

	private:
		std::unordered_map<std::string, std::shared_ptr<Renderable>> m_Meshes;
	};
}