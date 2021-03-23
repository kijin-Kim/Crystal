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
    
    /*TexCube�� �ݴ�� �����մϴ�.*/
    
    /*-1.0f ~ 1.0f�� ��ǥ��� ��ȯ ��*/
    float2 uv3D = uv2D * 2.0f - 1.0f;
    
    float3 normal;
    
    /*UV ��ǥ�迡�� Cubemap Texcoord ��ǥ��(?)�� ��ȯ*/
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
            /*���۷��� https://ko.wikipedia.org/wiki/%EA%B5%AC%EB%A9%B4%EC%A2%8C%ED%91%9C%EA%B3%84 */
            /*������ǥ�� -> ī�׽þ� ��ǥ�� (ź��Ʈ ����)*/
            float3 sampleInTangent = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
           /*ź��Ʈ ���� -> ���� ����*/ 
            float3 sampleInWorld = mul(sampleInTangent, tangentBasis);
            /*������ ���ʿ��� ���ø� �� ��� ���ø� ������ �������Ƿ� sin(theta)�� �����ϸ��Ͽ� �̸� �����մϴ�.*/
            //irradiance += CubemapTexture.SampleLevel(DefaultSampler, sampleInWorld, 0.0f).rgb * cos(theta) * sin(theta);
            irradiance += CubemapTexture.SampleLevel(DefaultSampler, sampleInWorld, 0.0f).rgb * cos(theta) * sin(theta);
            sampleCount ++;

        }
    }
    irradiance = PI * irradiance / sampleCount;
    OutputTexture[dispatchThreadID] = float4(irradiance, 1.0f);
    
}