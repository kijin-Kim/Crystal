#pragma once
#include <memory>

namespace Crystal {

	class Component;
	class TransformComponent;
	class MeshComponent;
	class Model;

	class Actor
	{
	public:
		Actor();
		virtual ~Actor();

		virtual void Start() {}
		virtual void Update(float DeltaTime);


	private:
		TransformComponent* m_MainComponent;
		std::vector<Component*> m_Components;

		//////////////////Temporary//////////////
		MeshComponent* m_MeshComponent = nullptr;
		Model* m_Model;
	};

}



