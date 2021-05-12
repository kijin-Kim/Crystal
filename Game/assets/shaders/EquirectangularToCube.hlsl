
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
    
    /*TexCube�� �ݴ�� �����մϴ�.*/
    
    /*-1.0f ~ 1.0f�� ��ǥ��� ��ȯ ��*/
    const float2 uv3D = 2.0f * uv2D - 1.0f;
    
    float3 cubeUV;
    
    /*UV ��ǥ�迡�� Cubemap Texcoord ��ǥ��(?)�� ��ȯ  ( �� ���ذ� �ʿ� ) */
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
    
    
    /*���۷��� https://ko.wikipedia.org/wiki/%EA%B5%AC%EB%A9%B4%EC%A2%8C%ED%91%9C%EA%B3%84 */
    /*ī�׽þ� ��ǥ�迡�� ������ǥ��� ��ȯ �� 0�� 1���̷� ���� (Noramlize)*/
    const float2 finalUV = float2(atan2(-cubeUV.x, -cubeUV.z) / TwoPI, acos(cubeUV.y) / PI);
    const float4 finalColor = EquiTexture.SampleLevel(DefaultSampler, finalUV, 0.0f);
    
    OutputTexture[dispatchThreadID] = finalColor;
}