
RWTexture2D<float4> BrightTexture : register(u0);

[numthreads(8, 8, 1)]
void csMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{  
    const float weight[5] = { 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 };
    
    float3 result = BrightTexture[dispatchThreadID.xy] * weight[0];

    if (fmod(dispatchThreadID.z, 2) == 0.0f) // horizontal
    {
        for(int i = 1; i < 5; i++)
        {
            result += BrightTexture[dispatchThreadID.xy + float2(i, 0)] * weight[i];
            result += BrightTexture[dispatchThreadID.xy - float2(i, 0)] * weight[i];
        }
    }
    else // vertical
    {
        for(int i = 1; i < 5; i++)
        {
            result += BrightTexture[dispatchThreadID.xy + float2(0, i)] * weight[i];
            result += BrightTexture[dispatchThreadID.xy - float2(0, i)] * weight[i];
        }
    }
    
    BrightTexture[dispatchThreadID.xy] = float4(result, 1.0f);
}