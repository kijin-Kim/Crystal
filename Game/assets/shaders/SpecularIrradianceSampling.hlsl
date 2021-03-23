//// based on https://github.com/Nadrin/PBR, and tutorial from https://learnopengl.com/PBR/IBL/Diffuse-irradiance

TextureCube CubemapTexture : register(t0);
RWTexture2DArray<float4> OutputTexture : register(u0);

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
    
    
}