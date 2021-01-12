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
    float4 WorldLightPosition : WORLD_LIGHT_POSITION;
    float4 WorldCameraPosition : WORLD_CAMERA_POSITION;
    float3 WorldNormal : NORMAL;
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
    
    PS_INPUT output;
    output.Position = mul(mul(float4(localPosition, 1.0f), World), ViewProjection);
    output.WorldPosition = mul(float4(localPosition, 1.0f), World);

    output.WorldLightPosition = WorldLightPosition;
    output.WorldCameraPosition = WorldCameraPosition;
    output.WorldNormal = mul(mul(float4(input.Normal, 1.0f), boneTransform), World).xyz;
    output.TexCoord = input.TexCoord;
    
    float3x3 TBN = float3x3(input.Tangent, input.BiTangent, input.Normal);
    output.TangentToWorld = mul(TBN, (float3x3)World);

    return output;
}




Texture2D AlbedoTexture : register(t0);
Texture2D MetalicTexture : register(t1);
Texture2D RoughnessTexture : register(t2);
Texture2D NormalTexture : register(t3);

SamplerState normalSampler : register(s0);

static const float PI = 3.14159265359;

float NormalDistributionGGX(float3 normal, float3 halfway, float roughness)
{
    //Trowbridge-Reitz GGX
    float roughnessSquared = roughness * roughness;
    float a2 = roughnessSquared * roughnessSquared;

    float dotNH = max(dot(normal, halfway), 0.0f);

    float numerator = a2;
    float denominator = ((dotNH * dotNH) * (a2 - 1.0f) + 1.0f);
    denominator = PI * denominator * denominator;

    return numerator / max(denominator, 0.001f);
}


float ndfGGX(float cosLh, float roughness)
{
	float alpha   = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

float GeometrySchlickGGX(float cosTheta, float roughness)
{
    float kDirect  = ((roughness + 1) * (roughness + 1)) / 8.0f; //ONLY FOR DIRECT LIGHT

    float numerator = cosTheta;
    float denominator = cosTheta * (1.0 - kDirect) + kDirect;

    return numerator / denominator;
}

float GeometrySmith(float dotNV, float dotNL, float roughness)
{
    return GeometrySchlickGGX(dotNV, roughness) * GeometrySchlickGGX(dotNL, roughness);
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return (F0 + (1.0f - F0) * pow(1.0 - cosTheta, 5.0f));
}


float4 psMain(PS_INPUT input) : SV_TARGET
{
    //Current we have only one directional light
    float3 lightColor = 10.0f;
    float3 albedo = bToggleAlbedoTexture ? pow(AlbedoTexture.Sample(normalSampler, input.TexCoord).rgb, float3(2.2f, 2.2f, 2.2f)) : float3(0.0f, 0.0f, 0.0f);
    float roughness = bToggleRoughnessTexture ? RoughnessTexture.Sample(normalSampler, input.TexCoord).r : 0.0f;
    float metallic = bToggleMetalicTexture ? MetalicTexture.Sample(normalSampler, input.TexCoord).r : 0.0f;
    int lightCount = 1;
    ////////////////

    float3 N = bToggleNormalTexture ? normalize(NormalTexture.Sample(normalSampler, input.TexCoord).rgb * 2.0f - 1.0f) : normalize(input.WorldNormal);
    N = normalize(mul(N, input.TangentToWorld));
    float3 V = normalize(input.WorldCameraPosition - input.WorldPosition).xyz;

    float3 F0 = 0.04f;
    F0 = lerp(F0, albedo, metallic);

    float3 Lo = 0.0f;
    for(int i= 0; i < lightCount; i++)
    {
        // L()
        float3 L = normalize(input.WorldLightPosition - input.WorldPosition).xyz;
        float3 H = normalize(V + L);
        float attenuation = 1.0f; // Directional Light
        float3 radiance = lightColor * attenuation;

        //Cook-Torrance BRDF
        float normalDistribution = NormalDistributionGGX(N, H, roughness);
        float geometry = GeometrySmith(max(dot(N,V), 0.0f), max(dot(N,L), 0.0f), roughness);
        float3 fresnel = FresnelSchlick(saturate(dot(H,V)), F0);


        float3 numerator = normalDistribution * geometry * fresnel;
        float denominator = 4 * max(dot(N,V), 0.0f) * max(dot(N,L), 0.0f);
        float3 specular = numerator / max(denominator, 0.001f);

        //Energy Conservation
        float3 specularEnergy = fresnel;
        float3 diffuseEnergy = float3(1.0f, 1.0f, 1.0f) - specularEnergy;
        //No Diffuse for Conductor
        diffuseEnergy *= 1.0f - metallic;
        
        Lo += (diffuseEnergy * albedo / PI + specular) * radiance * max(dot(N,L), 0.0f);
    }

    float3 ambient = float3(0.03f,0.03f,0.03f) * albedo;
    float3 finalColor = Lo + ambient;

    //HDR
    finalColor = finalColor / (finalColor + float3(1.0f, 1.0f, 1.0f));
    //Gamma Correction
    finalColor = pow(finalColor, float3(1.0f/2.2f, 1.0f/2.2f, 1.0f/2.2f));
    
    return float4(finalColor, 1.0f);
    
}