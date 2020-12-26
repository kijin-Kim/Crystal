

struct VS_INPUT
{
    float2 Position : POSITION;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
};


VS_OUTPUT vsMain(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = float4(input.Position, 1.0f, 1.0f);
    return output;
}