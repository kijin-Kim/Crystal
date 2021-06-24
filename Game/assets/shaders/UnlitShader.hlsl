

cbuffer PerFrameData : register(b0)
{
    float4x4 View : packoffset(c0);
    float4x4 Projection : packoffset(c4);
}

cbuffer PerDrawCallData : register(b1)
{
    uint HorizontalSubImageCount;
    uint VerticalSubImageCount;
}

struct VS_INPUT
{
    float3 Position : POSITION;
    float4 MatRow0 : MATROW0;
    float4 MatRow1 : MATROW1;
    float4 MatRow2 : MATROW2;
    float4 MatRow3 : MATROW3;
    uint SubUVIndex : SUBUV_INDEX;
};

struct PS_INPUT
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 MainColor : SV_Target0;
    float4 BrightColor : SV_Target1;
};



PS_INPUT vsMain(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;

    
    float4x4 World = float4x4(input.MatRow0, input.MatRow1, input.MatRow2, input.MatRow3);

    output.Position = mul(mul(mul(float4(input.Position, 1.0f), World), View), Projection);
    
    output.TexCoord = ((input.Position + 1.0f) / 2.0f);
	output.TexCoord.y = -output.TexCoord.y;

    output.TexCoord.x *= (1.0f / (float)HorizontalSubImageCount);
    output.TexCoord.y *= (1.0f / (float)VerticalSubImageCount);

    float xFactor = input.SubUVIndex % HorizontalSubImageCount;
    float yFactor = input.SubUVIndex / HorizontalSubImageCount;

    output.TexCoord.x += (1.0f / (float)HorizontalSubImageCount) * xFactor;
    output.TexCoord.y -= (1.0f / (float)VerticalSubImageCount) * ((VerticalSubImageCount - 1)- yFactor);

    

    return output;
}


SamplerState DefualtSampler : register(s0);
Texture2D Texture : register(t0);

PS_OUTPUT psMain(PS_INPUT input) : SV_Target
{
    float4 finalColor = Texture.Sample(DefualtSampler, input.TexCoord);

    float brightness = dot(finalColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));

    PS_OUTPUT output = (PS_OUTPUT)0;

    output.MainColor = finalColor;
    if(brightness > 1.0f)
        output.BrightColor = output.MainColor;
    else
        output.BrightColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

    return output;
}

