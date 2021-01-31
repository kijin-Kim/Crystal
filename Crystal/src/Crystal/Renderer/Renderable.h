#pragma once

namespace Crystal {

	class Renderable
	{
	public:
		Renderable() = default;
		virtual ~Renderable() = default;

		virtual void Update(float deltaTime) {}

	};
}
