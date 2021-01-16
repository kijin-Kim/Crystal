

struct VS_INPUT
{
    float2 Position : POSITION;    
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 TexCoord : TEXCOORD;
};

cbuffer Constants : register(b0)
{
    float4x4 InverseViewProj : packoffset(c0);
};


VS_OUTPUT vsMain(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = float4(input.Position, 1.0f, 1.0f);
    output.TexCoord = mul(output.Position, InverseViewProj).xyz;
    return output;
}