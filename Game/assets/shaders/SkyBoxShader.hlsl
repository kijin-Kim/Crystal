struct VS_INPUT
{
    float2 Position : POSITION;    
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 TexCoord : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 MainColor : SV_Target0;
    float4 BrightColor : SV_Target1;
};


cbuffer Constants : register(b0)
{
    float4x4 InverseViewProj : packoffset(c0);
};


PS_INPUT vsMain(VS_INPUT input)
{
    PS_INPUT output;
    output.Position = float4(input.Position, 1.0f, 1.0f);
    output.TexCoord = mul(output.Position, InverseViewProj).xyz;
    
    return output;
}

TextureCube CubemapColorTexture : register(t0);
SamplerState DefaultSampler : register(s0);

PS_OUTPUT psMain(PS_INPUT input) : SV_TARGET
{
    float3 color = CubemapColorTexture.Sample(DefaultSampler, input.TexCoord).rgb;

    float brightness = dot(color.rgb, float3(0.2126f, 0.7152f, 0.0722f));

    PS_OUTPUT output;

    output.MainColor = float4(color, 1.0f);

    if(brightness > 1.0f)
        output.BrightColor = output.MainColor;
    else
        output.BrightColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

    return output;
}