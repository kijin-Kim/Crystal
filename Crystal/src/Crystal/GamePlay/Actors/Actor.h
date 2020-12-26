#pragma once

namespace Crystal {

	class TransformComponent;
	class MeshComponent;
	class Mesh;

	class Actor
	{
	public:
		Actor();
		virtual ~Actor();

		virtual void Start() {}
		virtual void End() {} 
		virtual void Update(float deltaTime);
		void UpdateTransfromData();

	protected:
		//void CreateComponent() {}

	protected:
		TransformComponent* m_MainComponent = nullptr;

	private:
		std::vector<TransformComponent*> m_TransformComponents;

	};

}



