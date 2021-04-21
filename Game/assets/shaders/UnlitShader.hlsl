

cbuffer PerFrameData : register(b0)
{
    float4x4 View : packoffset(c0);
    float4x4 Projection : packoffset(c4);
}


struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;

    float4 MatRow0 : MATROW0;
    float4 MatRow1 : MATROW1;
    float4 MatRow2 : MATROW2;
    float4 MatRow3 : MATROW3;

    float3 Color : COLOR;
};

struct PS_INPUT
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD;

    nointerpolation float3 Color : COLOR;
};


PS_INPUT vsMain(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;

    float xScale = length(input.MatRow0);
    float yScale = length(input.MatRow1);
    float zScale = length(input.MatRow2);

    

    float3x3 viewRotation = View;
    float3x3 transposedViewRotation = transpose(viewRotation);
    
    float4 newRow0 = float4(transposedViewRotation[0], input.MatRow0[3]);
    float4 newRow1 = float4(transposedViewRotation[1], input.MatRow1[3]);
    float4 newRow2 = float4(transposedViewRotation[2], input.MatRow2[3]);
    
    float4x4 world = float4x4(newRow0, newRow1, newRow2, input.MatRow3);

    float4x4 worldView = mul(world, View);
    worldView[0][0] = xScale;
    worldView[1][1] = yScale;
    worldView[2][2] = zScale;
    
    output.Position = mul(mul(float4(input.Position, 1.0f), worldView), Projection);
    

    


    output.TexCoord = input.TexCoord;

    output.Color = input.Color;

    return output;
}


SamplerState DefualtSampler : register(s0);
#define MAX_PER_INSTANCE_TEXTURE_COUNT 10
Texture2D Textures[MAX_PER_INSTANCE_TEXTURE_COUNT][1] : register(t0);

float4 psMain(PS_INPUT input) : SV_Target
{
    float3 color = Textures[0][0].Sample(DefualtSampler, input.TexCoord).rgb;
    return float4(color, 1.0f);
}

