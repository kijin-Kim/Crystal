#pragma once
#include <unordered_map>

#include "Animation.h"
#include "Crystal/Core/Core.h"


namespace Crystal {

	class AnimationManager
	{
	public:
		AnimationManager() = default;
		~AnimationManager() = default;


		Weak<Animation> get(const std::string& fileName)
		{
			auto it = m_Animations.find(fileName);
			if (it != m_Animations.end())
			{
				return it->second;
			}

			auto newAnimation = CreateShared<Animation>(fileName);
			m_Animations.insert({ fileName, newAnimation });

			return newAnimation;
		}

	private:
		std::unordered_map<std::string, Shared<Animation>> m_Animations;
	};
}
