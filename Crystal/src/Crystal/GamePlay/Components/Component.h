#pragma once

namespace Crystal {

	class Component
	{
	public:
		Component() = default;
		virtual ~Component() = default;

		virtual void Update(float DeltaTime) {}


		void AttachToComponent(Component* parentComponent) { m_Parent = parentComponent; }

	private:
		Component* m_Parent = nullptr;
	};
}

