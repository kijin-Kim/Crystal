//this code is based on tutorial 
//https://learnopengl.com/PBR/Lighting, 
//https://learnopengl.com/PBR/IBL/Diffuse-irradiance

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiTangent : BITANGENT;
    uint4 BoneIDs : BONEIDS;
    float4 BoneWeights : BONEWEIGHTS;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION;
    float3 WorldNormal : WORLD_NORMAL;
    float2 TexCoord : TEXCOORD;
    float3x3 TangentToWorld : TANGENT_TO_WORLD;
};


cbuffer PerFrameData : register(b0)
{
    float4x4 ViewProjection : packoffset(c0);
    float4 WorldCameraPosition : packoffset(c4);
    float4 WorldLightPosition : packoffset(c5);
}

cbuffer PerObjectData : register(b1)
{
    float4x4 World;
    bool bToggleAlbedoTexture;
    bool bToggleMetalicTexture;
    bool bToggleRoughnessTexture;
    bool bToggleNormalTexture;
    float4 AlbedoColor;
    float RoughnessConstant;
    float MetalicConstant;
}
cbuffer MatrixPalette : register(b2)
{
    float4x4 Bones[52];
}


PS_INPUT vsMain(VS_INPUT input)
{
    float4x4 boneTransform = Bones[input.BoneIDs[0]] * input.BoneWeights[0];
    boneTransform += Bones[input.BoneIDs[1]] * input.BoneWeights[1];
    boneTransform += Bones[input.BoneIDs[2]] * input.BoneWeights[2];
    boneTransform += Bones[input.BoneIDs[3]] * (1.0f - (input.BoneWeights[2] + input.BoneWeights[1] + input.BoneWeights[0]));
    
    float3 localPosition = mul(float4(input.Position, 1.0f), boneTransform);
    //float3 localPosition = input.Position;
    PS_INPUT output;
    output.Position = mul(mul(float4(localPosition, 1.0f), World), ViewProjection);
    output.WorldPosition = mul(float4(localPosition, 1.0f), World);

    output.TexCoord = input.TexCoord;
    output.WorldNormal = mul(mul(input.Normal, (float3x3) boneTransform), (float3x3)World);
    //output.WorldNormal = mul(input.Normal, (float3x3)World);
    
    float3x3 TBN = float3x3(input.Tangent, input.BiTangent, input.Normal);
    output.TangentToWorld = mul(TBN, (float3x3)World);

    return output;
}




Texture2D AlbedoTexture : register(t0);
Texture2D MetalicTexture : register(t1);
Texture2D RoughnessTexture : register(t2);
Texture2D NormalTexture : register(t3);

SamplerState DefaultSampler : register(s0);

static const float PI = 3.14159265359;

float NormalDistributionGGX(float dotNH, float roughness)
{
    //Trowbridge-Reitz GGX
    float roughnessSquared = roughness * roughness;
    float a2 = roughnessSquared * roughnessSquared;

    float numerator = a2;
    float denominator = ((dotNH * dotNH) * (a2 - 1.0f) + 1.0f);
    denominator = PI * denominator * denominator;

    return numerator / max(denominator, 0.001f);
}


float GeometryShlickGGX(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

float GeometrySmith(float dotNV, float dotNL, float roughness)
{
    float r = roughness + 1.0;
    float kDirect = (r * r) / 8.0; //ONLY FOR DIRECTIONAL LIGHT
    return GeometryShlickGGX(dotNV, kDirect) * GeometryShlickGGX(dotNL, kDirect);
}


float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0 - cosTheta, 5.0f);
}


float4 psMain(PS_INPUT input) : SV_TARGET
{
    //Current we have only one directional light
    float3 lightColor = 10.0f;
    float3 albedo = bToggleAlbedoTexture ? pow(AlbedoTexture.Sample(DefaultSampler, input.TexCoord).rgb, float3(2.2f, 2.2f, 2.2f)) : AlbedoColor.rgb;
    float roughness = bToggleRoughnessTexture ? RoughnessTexture.Sample(DefaultSampler, input.TexCoord).r : RoughnessConstant;
    float metallic = bToggleMetalicTexture ? MetalicTexture.Sample(DefaultSampler, input.TexCoord).r : MetalicConstant;
    
    
    const int lightCount = 1; // ¿”Ω√

    /*≥Î∏ª*/
    float3 N = bToggleNormalTexture ? mul(normalize(NormalTexture.Sample(DefaultSampler, input.TexCoord).rgb * 2.0f - 1.0f), input.TangentToWorld) : input.WorldNormal;
    N = normalize(N);
    
    /*∫‰ ∫§≈Õ*/
    float3 V = normalize(WorldCameraPosition - input.WorldPosition).xyz;
    

    /*Base Reflectivity*/
    float3 F0 = 0.04f; 
    F0 = lerp(F0, albedo, metallic);

    
    float3 Lo = 0.0f;
    for(int i= 0; i < lightCount; i++)
    {
        // L()
        float3 L = normalize(WorldLightPosition - input.WorldPosition).xyz;
        float3 H = normalize(V + L);
        float attenuation = 1.0f; // Directional Light
        float3 radiance = lightColor * attenuation;
        
        float dotNV = max(dot(N, V), 0.0f); // Dot ( Normal, View )
        float dotNL = max(dot(N, L), 0.0f); // Dot ( Normal, OutgoingLight )
        float dotNH = max(dot(N,H), 0.0f); // Dot ( Normal, Halfway )
        
        //Cook-Torrance BRDF
        float normalDistribution = NormalDistributionGGX(dotNH, roughness);
        float geometry = GeometrySmith(dotNV, dotNL, roughness);
        float3 fresnel = FresnelSchlick(max(dot(H, V), 0.0f), F0);


        float3 numerator = normalDistribution * geometry * fresnel;
        float denominator = 4 * dotNV * dotNL;
        float3 specularBRDF = numerator / max(denominator, 0.001f);

        //Energy Conservation
        float3 specularEnergy = fresnel;
        float3 diffuseEnergy = float3(1.0f, 1.0f, 1.0f) - specularEnergy;
        //No Diffuse for Conductor
        diffuseEnergy *= 1.0f - metallic;
        
        Lo += (diffuseEnergy * albedo / PI + specularBRDF) * radiance * dotNL;
        //Lo += (diffuseEnergy * albedo + specularBRDF) * radiance * dotNL;
    }

    float3 ambient = float3(0.03f,0.03f,0.03f) * albedo;
    float3 finalColor = Lo + ambient;

    //HDR
    float3 hdrDenom = finalColor + 1.0f;
    finalColor = finalColor / hdrDenom;
    //Gamma Correction
    float3 gammaFactor = 1.0f / 2.2f;
    finalColor = pow(finalColor, gammaFactor);
    
    return float4(finalColor, 1.0f);
    
}