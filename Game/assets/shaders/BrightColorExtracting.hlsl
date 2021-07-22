Texture2D SourceTexture1 : register(t0);
RWTexture2D<float4> OutputTexture : register(u0);

[numthreads(8, 8, 1)]
void csMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{  
    float4 color = SourceTexture1[dispatchThreadID.xy];
    
    float brightness = dot(color.rgb, float3(0.2126f, 0.7152f, 0.0722f));

    if(brightness > 1.0f)
        OutputTexture[dispatchThreadID.xy] = color;
    else
        OutputTexture[dispatchThreadID.xy] = float4(0.0f, 0.0f, 0.0f, 1.0f);
}