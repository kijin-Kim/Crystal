

struct VS_INPUT
{
    float3 Position : POSITION;    
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 TexCoord : TEXCOORD;
};

cbuffer Constants : register(b0)
{
    float4x4 ViewProj : packoffset(c0);
};


VS_OUTPUT vsMain(VS_INPUT input)
{
    VS_OUTPUT output;
    output.TexCoord = input.Position;
    output.Position = mul(float4(input.Position, 0.0f), ViewProj);
    output.Position.z = output.Position.w;
    return output;
}