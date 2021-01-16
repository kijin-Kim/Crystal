#include "cspch.h"
#include "Pass.h"

namespace Crystal {


	void Pass::RegisterMesh(std::shared_ptr<MeshComponent>& meshComponent)
	{
		m_MeshComponents.push_back(meshComponent);
	}

	void Pass::UnRegisterMesh(MeshComponent* meshComponent)
	{
		//auto it = std::find(m_MeshComponents.begin(), m_MeshComponents.end(), meshComponent);
		//if (it == m_MeshComponents.end())
		//	CS_ASSERT(false, "삭제하려는 MeshComponent를 찾지 못하였습니다");

		///*erase copy를 방지합니다.*/
		//std::iter_swap(it, m_MeshComponents.end() - 1);
		//m_MeshComponents.pop_back();
	}

}



