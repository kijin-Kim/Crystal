

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



PS_INPUT vsMain(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;

    

    float3x3 transposedView = transpose(View);
    
    float4 newRow0 = float4(transposedView[0], 0.0f);
    float4 newRow1 = float4(transposedView[1], 0.0f);
    float4 newRow2 = float4(transposedView[2], 0.0f);
    
    float4x4 world = float4x4(newRow0, newRow1, newRow2, input.MatRow3);

    float4x4 worldView = mul(world, View);
    worldView[0][0] = length(input.MatRow0);
    worldView[1][1] = length(input.MatRow1);
    worldView[2][2] = length(input.MatRow2);
    
    output.Position = mul(mul(float4(input.Position, 1.0f), worldView), Projection);
    
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

float4 psMain(PS_INPUT input) : SV_Target
{
    float4 color = Texture.Sample(DefualtSampler, input.TexCoord);

    return float4(color.rgb, 1.0f);
}

