struct VS_INPUT
{
    float2 Position : POSITION;    
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};


PS_INPUT vsMain(VS_INPUT input)
{
    PS_INPUT output;
    output.Position = float4(input.Position, 1.0f, 1.0f);
    output.TexCoord = (output.Position + 1.0f) / 2.0f;
	output.TexCoord.y = -output.TexCoord.y;
    
    return output;
}


Texture2D FloatingPointTexture : register(t0);
SamplerState DefaultSampler : register(s0);

float4 psMain(PS_INPUT input) : SV_TARGET
{
    float3 color = FloatingPointTexture.Sample(DefaultSampler, input.TexCoord).rgb;

    //HDR Tonemapping
    // float3 hdrDenom = color + 1.0f;
    // color = color / hdrDenom;
	color = 1.0f - exp(-color * 0.3f);


    //Gamma Correction
    float3 gammaFactor = 1.0f / 2.2f;
    color = pow(color, gammaFactor);
    
    return float4(color, 1.0f);
}