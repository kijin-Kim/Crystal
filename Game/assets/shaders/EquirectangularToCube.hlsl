
////// EquiToCube is based on https://github.com/Nadrin/PBR

Texture2D EquiTexture : register(t0);
RWTexture2DArray<float4> OutputTexture : register(u0);
SamplerState DefaultSampler : register(s0);

static const float PI = 3.141592;
static const float TwoPI = 2 * PI;

[numthreads(32, 32, 1)]
void csMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    float outputWidth;
    float outputHeight;
    float outputDepth;
  
    OutputTexture.GetDimensions(outputWidth, outputHeight, outputDepth);

    const float2 uv2D = dispatchThreadID.xy / float2(outputWidth, outputHeight);
    
    /*TexCube를 반대로 진행합니다.*/
    
    /*-1.0f ~ 1.0f인 좌표계로 변환 후*/
    const float2 uv3D = 2.0f * uv2D - 1.0f;
    
    float3 cubeUV;
    
    /*UV 좌표계에서 Cubemap Texcoord 좌표계(?)로 변환  ( 더 이해가 필요 ) */
    switch (dispatchThreadID.z) 
    {
    case 0: cubeUV = float3(+1.0f, -uv3D.y, -uv3D.x); break;
    case 1: cubeUV = float3(-1.0f, -uv3D.y, +uv3D.x); break;
    case 2: cubeUV = float3(+uv3D.x, +1.0f, +uv3D.y); break;
    case 3: cubeUV = float3(+uv3D.x, -1.0f, -uv3D.y); break;
    case 4: cubeUV = float3(+uv3D.x, -uv3D.y, +1.0f); break;
    case 5: cubeUV = float3(-uv3D.x, -uv3D.y, -1.0f); break;
    }
    
    cubeUV = normalize(cubeUV);
    
    
    /*레퍼런스 https://ko.wikipedia.org/wiki/%EA%B5%AC%EB%A9%B4%EC%A2%8C%ED%91%9C%EA%B3%84 */
    /*카테시안 좌표계에서 구면좌표계로 변환 및 0과 1사이로 매핑 (Noramlize)*/
    const float2 finalUV = float2(atan2(-cubeUV.x, -cubeUV.z) / TwoPI, acos(cubeUV.y) / PI);
    const float4 finalColor = EquiTexture.SampleLevel(DefaultSampler, finalUV, 0.0f);
    
    OutputTexture[dispatchThreadID] = finalColor;
}