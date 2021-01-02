#pragma once
#include "Crystal/GamePlay/Components/MeshComponent.h"

namespace Crystal {

	struct PassParams
	{
		
	};

	class Pass
	{
	public:
		Pass() = default;
		virtual ~Pass() = default;

		void Begin(const PassParams& params) {}
		void Excute() {}
		void End() {}

	protected:
		std::vector<MeshComponent*> m_MeshComponents;
	};

}
