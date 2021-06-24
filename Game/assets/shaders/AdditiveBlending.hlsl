
Texture2D SourceTexture1 : register(t0);
RWTexture2D<float4> OutputTexture : register(u0);

[numthreads(8, 8, 1)]
void csMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{  

    float4 finalColor = OutputTexture[dispatchThreadID.xy];
    finalColor += SourceTexture1[dispatchThreadID.xy];

    OutputTexture[dispatchThreadID.xy] = finalColor;
}