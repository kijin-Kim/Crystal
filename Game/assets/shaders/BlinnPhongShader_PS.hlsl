Texture2D DiffuseMap : register(t0);
SamplerState NormalSampler : register(s0);


struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 PositionInWorld : POSITION;
    float4 LightPositionInWorld : POSITION1;
    float4 CameraPositionInWorld : POSITION2;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

float4 psMain(VS_OUTPUT input) : SV_TARGET
{
    float3 lightColor = float3(1.0f,1.0f,1.0f);
    // Diffuse Map
    float4 diffuseColor = DiffuseMap.Sample(NormalSampler, input.TexCoord);

    // Ambient Light
    float ambientPower = 0.5f;
    float3 ambientLight = diffuseColor.xyz * ambientPower;

    // Diffuse Light
    float diffusePower = 0.7f;
    float3 normal = normalize(input.Normal);
    float3 lightDirection = normalize(input.PositionInWorld - input.LightPositionInWorld).xyz;
    float diffuseFactor = max(dot(input.Normal, -lightDirection), 0.0f);
    float3 diffuseLight = diffuseFactor * diffuseColor.xyz * diffusePower;

    // Specular Light
    float specularPower = 1.0f;
    float3 viewDirection = normalize(input.PositionInWorld - input.CameraPositionInWorld).xyz;
    float3 halfwayDirection = normalize(-lightDirection + -viewDirection);
    float specularFactor = pow(max(dot(input.Normal, halfwayDirection), 0.0f), 32 * 8);
    float3 specularLight = specularFactor * lightColor * specularPower; 

    
    // Result Color
    float4 resultColor = float4(ambientLight + diffuseLight + specularLight, 1.0f);
    return resultColor;
}