
Texture2D SourceTexture1 : register(t0);
RWTexture2D<float4> OutputTexture : register(u0);


[numthreads(8, 8, 1)]
void csMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{  
    float sourceWidth;
    float sourceHeight;
    SourceTexture1.GetDimensions(sourceWidth, sourceHeight);

    float outputWidth;
    float outputHeight;
    OutputTexture.GetDimensions(outputWidth, outputHeight);

    float2 sourceCoord = dispatchThreadID.xy * float2(sourceWidth, sourceHeight) / float2(outputWidth, outputHeight);


    float4 sourceColor = SourceTexture1[sourceCoord.xy];
    sourceColor += OutputTexture[dispatchThreadID.xy];
    OutputTexture[dispatchThreadID.xy] = sourceColor;
}