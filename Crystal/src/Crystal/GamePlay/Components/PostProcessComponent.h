#pragma once
#include "PrimitiveComponent.h"

namespace Crystal {

	class PostProcessComponent : public PrimitiveComponent
	{
	public:
		PostProcessComponent() = default;
		~PostProcessComponent() override = default;

		void RegisterComponent() override;


		void SetPostProcessOrder(uint32_t order) { m_PostProcessOrder = order; }
		uint32_t GetPostProcessOrder() const { return m_PostProcessOrder; }

		void SetIsEnabled(bool bIsEnabled) { m_bIsEnabled = bIsEnabled; }
		bool GetIsEnabled() const { return m_bIsEnabled; }

		STATIC_TYPE_IMPLE(PostProcessComponent)

	private:
		uint32_t m_PostProcessOrder = 0;

		bool m_bIsEnabled = true;
	};
}


