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
		//	CS_ASSERT(false, "�����Ϸ��� MeshComponent�� ã�� ���Ͽ����ϴ�");

		///*erase copy�� �����մϴ�.*/
		//std::iter_swap(it, m_MeshComponents.end() - 1);
		//m_MeshComponents.pop_back();
	}

}



