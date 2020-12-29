#pragma once

namespace Crystal {
	class Component
	{
	public:
		Component() = default;
		virtual ~Component() = default;

		virtual void Update(float deltaTime) {}
	};
}
