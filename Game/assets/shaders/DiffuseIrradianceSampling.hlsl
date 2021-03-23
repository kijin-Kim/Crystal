//// based on https://github.com/Nadrin/PBR, and tutorial from https://learnopengl.com/PBR/IBL/Diffuse-irradiance

TextureCube CubemapTexture : register(t0);
RWTexture2DArray<float4> OutputTexture : register(u0);
SamplerState DefaultSampler : register(s0);

static const float PI = 3.141592;

[numthreads(32, 32, 1)]
void csMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    float outputWidth;
    float outputHeight;
    float outputDepth;
  
    OutputTexture.GetDimensions(outputWidth, outputHeight, outputDepth);

    float2 uv2D = dispatchThreadID.xy / float2(outputWidth, outputHeight);
    
    /*TexCube를 반대로 진행합니다.*/
    
    /*-1.0f ~ 1.0f인 좌표계로 변환 후*/
    float2 uv3D = uv2D * 2.0f - 1.0f;
    
    float3 normal;
    
    /*UV 좌표계에서 Cubemap Texcoord 좌표계(?)로 변환*/
   switch (dispatchThreadID.z) 
   {
    case 0: normal = float3(+1.0f, -uv3D.y, -uv3D.x); break;
    case 1: normal = float3(-1.0f, -uv3D.y, +uv3D.x); break;
    case 2: normal = float3(+uv3D.x, +1.0f, +uv3D.y); break;
    case 3: normal = float3(+uv3D.x, -1.0f, -uv3D.y); break;
    case 4: normal = float3(+uv3D.x, -uv3D.y, +1.0f); break;
    case 5: normal = float3(-uv3D.x, -uv3D.y, -1.0f); break;
   }
    
    normal = normalize(normal);
    
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 right = cross(up, normal);
    up = cross(normal, right);
    
    float3x3 tangentBasis = float3x3(right, up, normal);
    
    float sampleCount = 0.0f;
    float3 irradiance = 0.0f;
    for (float phi = 0.0f; phi < 2 * PI; phi += 0.025)
    {
        for (float theta = 0.0f; theta < 0.5f * PI; theta += 0.01)
        {
            /*레퍼런스 https://ko.wikipedia.org/wiki/%EA%B5%AC%EB%A9%B4%EC%A2%8C%ED%91%9C%EA%B3%84 */
            /*구면좌표계 -> 카테시안 좌표계 (탄젠트 공간)*/
            float3 sampleInTangent = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
           /*탄젠트 공간 -> 월드 공간*/ 
            float3 sampleInWorld = mul(sampleInTangent, tangentBasis);
            /*구면의 위쪽에서 샘플링 할 경우 샘플링 간격이 좁아지므로 sin(theta)로 스케일링하여 이를 보완합니다.*/
            //irradiance += CubemapTexture.SampleLevel(DefaultSampler, sampleInWorld, 0.0f).rgb * cos(theta) * sin(theta);
            irradiance += CubemapTexture.SampleLevel(DefaultSampler, sampleInWorld, 0.0f).rgb * cos(theta) * sin(theta);
            sampleCount ++;

        }
    }
    irradiance = PI * irradiance / sampleCount;
    OutputTexture[dispatchThreadID] = float4(irradiance, 1.0f);
    
}