#pragma once
#include <memory>

namespace Crystal {

	// Shared pointer cast helper
	template<class TargetType, class SourceType>
	inline std::shared_ptr<TargetType> Cast(std::shared_ptr<SourceType> pt)
	{
		return std::static_pointer_cast<TargetType>(pt);
	}

	template<class TargetType, class SourceType>
	inline std::shared_ptr<TargetType> Cast(std::weak_ptr<SourceType> weak)
	{
		std::shared_ptr<SourceType> strong = weak.lock();
		if(!strong) // Strong pointer가 소멸했을 경우
			return nullptr;

		return std::static_pointer_cast<TargetType>(strong);
	}

}

