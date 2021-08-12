cbuffer perExecuteData : register(b0)
{
    float3 AlbedoColor;
    float Opacity;
    float3 TintColor;
    float OpacityMultiplier;
    float2 TexCoordMultiplier;

    bool bToggleAlbedoTexture;
    bool bToggleOpacityTexture;
    bool bToggleSceneColorTexture;
}


Texture2D AlbedoTexture : register(t0);
Texture2D OpacityTexture : register(t1);
Texture2D SceneColorMaskTexture : register(t2);
RWTexture2D<float4> OutputTexture : register(u0);

SamplerState DefaultSampler : register(s0);

[numthreads(8, 8, 1)]
void csMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    
    uint outputWidth;
    uint outputHeight;
    OutputTexture.GetDimensions(outputWidth, outputHeight);

    float2 uvCoord = dispatchThreadID.xy / float2(outputWidth, outputHeight);
    uvCoord *= TexCoordMultiplier;

    float4 sceneColor = OutputTexture[dispatchThreadID.xy];


    float4 albedoColor = bToggleAlbedoTexture ? float4(AlbedoTexture.SampleLevel(DefaultSampler, uvCoord.xy, 0.0f).rgb, 0.0f) : float4(AlbedoColor, 0.0f);
    float opacity = bToggleOpacityTexture ? OpacityTexture.SampleLevel(DefaultSampler, uvCoord.xy, 0.0f).r : Opacity;
    opacity *= OpacityMultiplier;

    albedoColor = lerp(sceneColor, albedoColor, opacity);

    float4 finalColor = sceneColor + albedoColor;
    finalColor *= float4(TintColor.rgb, 1.0f);
    
    if(bToggleSceneColorTexture)
    {
        float mask = SceneColorMaskTexture.SampleLevel(DefaultSampler, uvCoord.xy, 0.0f).r;
        finalColor *= mask;
    }
    
    OutputTexture[dispatchThreadID.xy] = finalColor;
}