#pragma once
#include "Crystal/GamePlay/Components/MeshComponent.h"

namespace Crystal {

	class Pass
	{
	public:
		Pass() = default;
		virtual ~Pass() = default;

		
		virtual void Excute() {}

	
		void RegisterMesh(std::shared_ptr<MeshComponent>& meshComponent);
		void UnRegisterMesh(MeshComponent* meshComponent);

	protected:
		std::vector<std::shared_ptr<MeshComponent>> m_MeshComponents;
	};
}
