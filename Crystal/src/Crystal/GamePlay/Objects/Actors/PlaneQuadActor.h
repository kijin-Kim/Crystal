#pragma once
#include "Actor.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {
	class PlaneQuadActor : public Actor
	{

		
	public:
		PlaneQuadActor()
		{
			m_StaticMeshComponent = CreateComponent<StaticMeshComponent>("StaticMeshComponent");
			//m_StaticMeshComponent->SetRenderable(resourceManager.GetRenderable("2DPlaneQuadMesh"));

			m_MainComponent = m_StaticMeshComponent;

		}

		~PlaneQuadActor() override = default;


		STATIC_TYPE_IMPLE(PlaneQuadActor)

	protected:
		std::shared_ptr<StaticMeshComponent> m_StaticMeshComponent = nullptr;
	};



}


