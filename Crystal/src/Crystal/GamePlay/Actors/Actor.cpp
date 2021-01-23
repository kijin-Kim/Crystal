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
	}

	void Actor::Update(float DeltaTime)
	{
		m_MainComponent->Update(DeltaTime);
		for (auto& component : m_TransformComponents)
			component->Update(DeltaTime);
	}
}