#include "cspch.h"
#include "Actor.h"

#include "Crystal/GamePlay/Components/MeshComponent.h"
#include "Crystal/Renderer/Model.h"

namespace Crystal {

	Actor::Actor() :
		m_MainComponent(new Component())
	{
		////// TEMPORARY ////
		m_MeshComponent = new MeshComponent();
		m_MeshComponent->SetMesh(std::make_shared<Model>("assets/models/Megaphone_01.fbx"));
		m_MeshComponent->AttachToComponent(m_MainComponent);
	}

	Actor::~Actor()
	{
		//Component Liftime is with Actor
		while (!m_Components.empty())
		{
			delete m_Components.back();
			m_Components.pop_back();
		}

		if(m_MainComponent)
			delete m_MainComponent;
	}

	void Actor::Update(float DeltaTime)
	{
		CS_LOG("Actor is Updating");
		//Update All Components!
		m_MainComponent->Update(DeltaTime);
		for (Component* component : m_Components)
			component->Update(DeltaTime);
	}

}