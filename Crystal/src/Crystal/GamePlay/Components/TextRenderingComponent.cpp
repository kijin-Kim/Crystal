#include "cspch.h"
#include "TextRenderingComponent.h"

#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Scene.h"

namespace Crystal {


	void TextRenderingComponent::RegisterComponent()
	{
		PrimitiveComponent::RegisterComponent();

		auto owner = Cast<Actor>(GetOuter());
		auto level = Cast<Level>(owner->GetOuter());
		auto scene = level->GetScene();

		scene->Texts.push_back(Cast<TextRenderingComponent>(shared_from_this()));
	}
}
