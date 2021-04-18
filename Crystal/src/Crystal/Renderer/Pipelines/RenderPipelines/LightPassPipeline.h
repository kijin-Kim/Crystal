#pragma once
#include "LightPipeline.h"

namespace Crystal {

	class LightPassPipeline : public LightPipeline
	{
	public:

		struct Light
		{
			DirectX::XMFLOAT3 WorldPosition = Vector3::Zero;
			float _pad1;

			DirectX::XMFLOAT3 Color = Vector3::White;
			float _pad2;

			float Intensity;
			float _pad3[3];
		};

		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 ViewProjection = Matrix4x4::Identity();

			DirectX::XMFLOAT3 CameraPositionInWorld = Vector3::Zero;
			float _pad1;

			Light Lights[100];

			uint32_t LightCount = 0;
			float _pad2[3]; // just in case
		};


	public:
		LightPassPipeline() = default;
		~LightPassPipeline() override = default;


		void OnCreate() override;;
		void Begin(const PipelineInputs* const pipelineInputs) override;
	};


}
