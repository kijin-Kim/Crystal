#include "cspch.h"
#include "PostProcessVolumeActor.h"

namespace Crystal {

	void PostProcessVolumeActor::SetPostProcessOrder(uint32_t order)
	{
		m_PostProcessComponent->SetPostProcessOrder(order);
	}

	uint32_t PostProcessVolumeActor::GetPostProcessOrder() const
	{
		return m_PostProcessComponent->GetPostProcessOrder();
	}

	void PostProcessVolumeActor::SetHiddenInGame(bool bHiddenInGame)
	{
		m_PostProcessComponent->SetHiddenInGame(bHiddenInGame);
	}

	bool PostProcessVolumeActor::GetHiddenInGame() const
	{
		return m_PostProcessComponent->GetHiddenInGame();
	}

	void PostProcessVolumeActor::OnBeginOverlap(const OverlapResult& overlapResult)
	{
		m_bIsFirstTimeCheckOverlap = false;
		m_bIsOverlapped = true;

		if(m_bIsEnabled)
		{
			switch (m_VolumeBehavior)
			{
			case EVolumeBehavior::VB_EnableWhenOverlap:
				SetHiddenInGame(false);
				break;
			case EVolumeBehavior::VB_EnableWhenNotOverlap:
				SetHiddenInGame(true);
				break;
			}
		}

		
	}

	
	void PostProcessVolumeActor::OnEndOverlap(const OverlapResult& overlapResult)
	{
		m_bIsFirstTimeCheckOverlap = false;
		m_bIsOverlapped = false;

		if(m_bIsEnabled)
		{
			switch (m_VolumeBehavior)
			{
			case EVolumeBehavior::VB_EnableWhenOverlap:
				SetHiddenInGame(true);
				break;
			case EVolumeBehavior::VB_EnableWhenNotOverlap:
				SetHiddenInGame(false);
				break;
			}
		}
		
	}

	
	bool PostProcessVolumeActor::GetIsOverlapped() const
	{
		return m_bIsOverlapped;
	}
}
