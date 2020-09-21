#pragma once
#include <DirectXMath.h>
#include "Crystal/GamePlay/Components/Component.h"

namespace Crystal {

	class TransformComponent : public Component
	{
	public:
		TransformComponent() = default;
		virtual ~TransformComponent() = default;

		DirectX::XMFLOAT4X4 WorldTransform;
	};

}

