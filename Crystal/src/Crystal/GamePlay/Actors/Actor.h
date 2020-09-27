#pragma once
#include <memory>

namespace Crystal {

	class Component;
	class MeshComponent;
	class Model;

	class Actor
	{
	public:
		Actor();
		virtual ~Actor();

		virtual void Start() {}
		virtual void Update(float DeltaTime);

		MeshComponent* GetMeshComponent() { return m_MeshComponent; }

	private:
		Component* m_MainComponent;
		std::vector<Component*> m_Components;

		//////////////////Temporary//////////////
		// Need to be memeber of some specialized child class e.g. MeshActor
		MeshComponent* m_MeshComponent = nullptr;
	};

}



