#pragma once
#include <unordered_map>
#include <memory>
#include "Renderable.h"

namespace Crystal {
	class RenderableManager
	{
		friend class ResourceManager;
	private:
		template<class T, class... Args>
		void createFromFile(const std::string& alias, Args... args)
		{
			std::string name;
			name = alias;

			if (m_Meshes.find(name) != m_Meshes.end())
			{
				CS_FATAL(false, "%s이름을 가진 Renderable이 이미 존재합니다", name.c_str());
			}

			m_Meshes[name] = std::make_shared<T>(std::forward<Args>(args)...);
		}

		template<class T>
		void create(const std::string& name)
		{
			if (m_Meshes.find(name) != m_Meshes.end())
			{
				CS_FATAL(false, "%s이름을 가진 Renderable이 이미 존재합니다", name.c_str());
			}

			m_Meshes[name] = std::make_shared<T>();
		}

		void destroy(const std::string& name)
		{
			auto it = m_Meshes.find(name);
			if (it == m_Meshes.end())
				CS_FATAL(false, "Mesh: %s를 찾을 수 없습니다.", name.c_str());
			m_Meshes.erase(name);
		}

		std::weak_ptr<Renderable> get(const std::string& name)
		{
			auto it = m_Meshes.find(name);
			if (it == m_Meshes.end())
				CS_FATAL(false, "Renderable: %s를 찾을 수 없습니다.", name.c_str());
			return m_Meshes[name];
		}

	private:
		RenderableManager() = default;
		~RenderableManager() = default;

	private:
		std::unordered_map<std::string, std::shared_ptr<Renderable>> m_Meshes;
	};
}
