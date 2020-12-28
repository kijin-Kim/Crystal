#include "cspch.h"
#include "Actor.h"

#include "Crystal/GamePlay/Components/MeshComponent.h"
#include "Crystal/Renderer/Mesh.h"

namespace Crystal {

	Actor::Actor()
	{

	}

	Actor::~Actor()
	{
		//Component Liftime is with Actor
		while (!m_TransformComponents.empty())
		{
			delete m_TransformComponents.back();
			m_TransformComponents.pop_back();
		}

		if(m_MainComponent)
			delete m_MainComponent;
	}

	void Actor::Update(float DeltaTime)
	{
		m_MainComponent->Update(DeltaTime);
		for (TransformComponent* component : m_TransformComponents)
				component->Update(DeltaTime);
	}

	void Actor::UpdateTransfromData()
	{
		for (TransformComponent* component : m_TransformComponents)
			component->UpdateConstantBuffer();
	}

}