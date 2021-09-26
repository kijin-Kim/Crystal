#pragma once
#include "PrimitiveComponent.h"

namespace Crystal {

	enum class TextAlignment
	{
		TA_Default,
		TA_Center
	};

	class TextRenderingComponent : public PrimitiveComponent
	{

	public:
		TextRenderingComponent() = default;
		~TextRenderingComponent() override = default;

		void RegisterComponent() override;

		void SetText(const std::wstring text) { m_Text = text; }
		void SetSize(uint32_t size) { m_Size = size; }
		void SetTextColor(const DirectX::XMFLOAT3& color) { m_Color = color; }
		void SetHorizontalTextAlignment(TextAlignment textAlignment) { m_HorizontalTextAlignment = textAlignment; }
		void SetVerticalTextAlignment(TextAlignment textAlignment) { m_VerticalTextAlignment = textAlignment; }

		const std::wstring& GetText() const { return m_Text; }
		uint32_t GetSize() const { return m_Size; }
		const DirectX::XMFLOAT3& GetTextColor() const { return m_Color; }
		TextAlignment GetVerticalTextAlignment() const {  return m_VerticalTextAlignment; }
		TextAlignment GetHorizontalTextAlignment() const {  return m_HorizontalTextAlignment; }
		

		STATIC_TYPE_IMPLE(TextRenderingComponent)

	private:
		std::wstring m_Text;
		uint32_t m_Size;
		DirectX::XMFLOAT3 m_Color = Crystal::Vector3::White;
		TextAlignment m_HorizontalTextAlignment = TextAlignment::TA_Default;
		TextAlignment m_VerticalTextAlignment = TextAlignment::TA_Default;
		

	};
}
