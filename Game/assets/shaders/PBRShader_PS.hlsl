

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION;
    float4 WorldLightPosition : POSITION1;
    float4 WorldCameraPosition : POSITION2;
    float3 WorldNormal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

Texture2D DiffuseMap : register(t0);
SamplerState NormalSampler : register(s0);

static const float PI = 3.14159265359;

float NormalDistributionGGX(float3 normal, float3 halfway, float roughness)
{
    //Trowbridge-Reitz GGX
    float dotNH = max(dot(normal, halfway), 0.0f);
    return (roughness * roughness) / max(PI * (((dotNH * dotNH) * (roughness * roughness - 1) + 1)  * ((dotNH * dotNH) * (roughness * roughness - 1) + 1)),0.001f);
}

float GeometrySchlickGGX(float3 dotNV, float roughness)
{
    float kDirect  = (roughness + 1) * (roughness + 1) / 8.0f; //ONLY FOR DIRECT LIGHT
    return dotNV / (dotNV * (1 - kDirect) + kDirect);
}

float GeometrySmith(float3 dotNV, float3 dotNL, float roughness)
{
    return GeometrySchlickGGX(dotNV, roughness) * GeometrySchlickGGX(dotNL, roughness);
}

float3 fresnelSchlick(float dotHV, float3 F0)
{
    return F0 + (1.0 - F0) * pow((1 - dotHV), 5.0f);
}


float4 psMain(VS_OUTPUT input) : SV_TARGET
{
    //Current we have only one directional light

    //F0 for dielectric 
    float3 albedo = float3(0.98, 0.97, 0.95);
    float metallic = 1.0f;
    float3 F0 = 0.04f;
    F0 = lerp(F0, albedo, metallic); 

    float3 Lo = 0.0f;

    float roughness = 0.1f;
    
    float3 lightDirection = normalize(input.WorldPosition - input.WorldLightPosition).xyz;
    float3 cameraDirection = normalize(input.WorldPosition - input.WorldCameraPosition).xyz;
    float3 halfway = normalize(-lightDirection + -cameraDirection);
    float3 normal = normalize(input.WorldNormal);

    // Radiance L()
    float distance = length(input.WorldLightPosition - input.WorldPosition);
    float attenuation = 1.0f / (distance * distance);
    float3 radiance = float3(1.0f, 1.0f, 1.0f);

    // Cook-Torrance BRDF f()
    float normalDistribution = NormalDistributionGGX(normal, halfway, roughness); //D
    float geometry = GeometrySmith(max(dot(normal, -cameraDirection), 0.0f), max(dot(normal, -lightDirection), 0.0f), roughness); //G
    float3 fresnel = fresnelSchlick(max(dot(halfway, -cameraDirection), 0.0f), F0);

    float3 specular = (normalDistribution * geometry * fresnel) / max(4 * max(dot(normal, -cameraDirection), 0.0f) * max(dot(normal, -lightDirection), 0.0f), 0.001f);

    //Energy Conservation
    float3 specularFraction = fresnel;
    float3 diffuseFraction = float3(1.0f, 1.0f, 1.0f) - specularFraction;
    //Conductor Has no diffuse
    diffuseFraction *= 1.0f - metallic;

    float dotNL = max(dot(normal, -lightDirection), 0.0f);
    
    Lo += (diffuseFraction * float3(1.0f, 1.0f, 1.0f) / PI + specular) * radiance * dotNL;

    float3 ambient = float3(0.03f, 0.03f, 0.03f) * float3(1.0f, 1.0f, 1.0f);

    float3 finalColor = Lo + ambient;
    finalColor = finalColor / (finalColor + float3(1.0f, 1.0f, 1.0f));
    finalColor = pow(finalColor, float3(1.0f/2.2f,1.0f/2.2f,1.0f/2.2f));


    return float4(finalColor, 1.0f);
}