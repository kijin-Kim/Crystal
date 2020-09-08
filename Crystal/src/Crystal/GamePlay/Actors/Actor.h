#pragma once
#include <memory>

namespace Crystal {

	class Level;
	class Component;
	class TransformComponent;
	class MeshComponent;
	class Model;

	class Actor
	{
	public:
		Actor();
		~Actor();

		virtual void Start() {}
		virtual void Update(float DeltaTime);

		void SetOwningLevel(Level* level) { m_OwningLevel = level; }
		Level* GetOwningLevel() { return m_OwningLevel; }

	private:
		TransformComponent* m_MainComponent;
		std::vector<Component*> m_Components;

		Level* m_OwningLevel = nullptr;

		//////////////////Temporary//////////////
		MeshComponent* m_MeshComponent = nullptr;
		Model* m_Model;
	};

}



