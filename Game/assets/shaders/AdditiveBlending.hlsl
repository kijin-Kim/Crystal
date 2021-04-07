
Texture2D SourceTexture1 : register(t0);
RWTexture2D<float4> OutputTexture : register(u0);

[numthreads(8, 8, 1)]
void csMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{  

    float3 finalColor = OutputTexture[dispatchThreadID.xy];
    finalColor += SourceTexture1[dispatchThreadID.xy];
   

    OutputTexture[dispatchThreadID.xy] = float4(finalColor, 1.0f);
}