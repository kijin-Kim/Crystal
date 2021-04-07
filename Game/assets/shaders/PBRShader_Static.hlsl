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
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION;
    float3 WorldNormal : WORLD_NORMAL;
    float2 TexCoord : TEXCOORD;
    float3 WorldTangent : WORLD_TANGENT;
    float3 WorldBiTangent : WORLD_BITANGENT;
};

struct PS_OUTPUT
{
    float4 MainColor : SV_Target0;
    float4 BrightColor : SV_Target1;
};


struct Light
{
    float4 WorldPosition;
    float4 Color;
    float Intensity;
    float3 padding;
};


cbuffer PerFrameData : register(b0)
{
    float4x4 ViewProjection : packoffset(c0);
    float4 WorldCameraPosition : packoffset(c4);
    Light Lights[20] : packoffset(c5);
    int LightCount : packoffset(c65);
}

cbuffer PerObjectData : register(b1)
{
    float4x4 World;
}

cbuffer PerMeshData : register(b2)
{
    float3 AlbedoColor : packoffset(c0);
    float3 EmissiveColor : packoffset(c1);
    bool bToggleAlbedoTexture : packoffset(c2.x);
    bool bToggleMetallicTexture : packoffset(c2.y);
    bool bToggleRoughnessTexture : packoffset(c2.z);
    bool bToggleNormalTexture : packoffset(c2.w);
    bool bToggleIrradianceTexture : packoffset(c3.x);
    bool bToggleEmissivetexture : packoffset(c3.y);
    float RoughnessConstant : packoffset(c3.z);
    float MetallicConstant  : packoffset(c3.w);
}

PS_INPUT vsMain(VS_INPUT input)
{
    PS_INPUT output;
    output.Position = mul(mul(float4(input.Position, 1.0f), World), ViewProjection);
    output.WorldPosition = mul(float4(input.Position, 1.0f), World);

    output.TexCoord = input.TexCoord;
    output.WorldNormal = mul(input.Normal, (float3x3) World);
    
    output.WorldTangent = mul(input.Tangent, (float3x3) World);
    output.WorldBiTangent = mul(input.BiTangent, (float3x3) World);

    return output;
}



/*Always*/
TextureCube IrradianceTexture : register(t0);

/*PerObject*/
Texture2D AlbedoTexture : register(t1);
Texture2D MetallicTexture : register(t2);
Texture2D RoughnessTexture : register(t3);
Texture2D NormalTexture : register(t4);
Texture2D EmissiveTexture : register(t5);


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
    return F0 + (1.0f - F0) * pow(max(1.0f - cosTheta, 0.0f), 5.0f);
}


float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(1.0f - roughness, F0) - F0) * pow(max(1.0f - cosTheta, 0.0f), 5.0f);
}


PS_OUTPUT psMain(PS_INPUT input) : SV_TARGET
{
    //Current we have only one directional light
    float3 albedo = bToggleAlbedoTexture ? pow(AlbedoTexture.Sample(DefaultSampler, input.TexCoord).rgb, float3(2.2f, 2.2f, 2.2f)) : AlbedoColor.rgb;
    float roughness = bToggleRoughnessTexture ? RoughnessTexture.Sample(DefaultSampler, input.TexCoord).r : RoughnessConstant;
    float metallic = bToggleMetallicTexture ? MetallicTexture.Sample(DefaultSampler, input.TexCoord).r : MetallicConstant;
    float3 emissive = bToggleEmissivetexture ? EmissiveTexture.Sample(DefaultSampler, input.TexCoord).rgb : EmissiveColor;
    

    /*�븻*/
    float3 N = bToggleNormalTexture ? mul(normalize(NormalTexture.Sample(DefaultSampler, input.TexCoord).rgb * 2.0f - 1.0f),
    float3x3(normalize(input.WorldTangent), normalize(input.WorldBiTangent), normalize(input.WorldNormal))) : input.WorldNormal;
    N = normalize(N);

    
    /*�� ����*/
    float3 V = normalize(WorldCameraPosition - input.WorldPosition).xyz;
    
    /*Base Reflectivity*/
    float3 F0 = 0.04f;
    F0 = lerp(F0, albedo, metallic);

    
    float3 Lo = 0.0f;
    for (int i = 0; i < LightCount; i++)
    {  
        float3 finalLightColor = Lights[i].Color * Lights[i].Intensity;

        // L()
        float3 L = normalize(Lights[i].WorldPosition - input.WorldPosition).xyz;
        float3 H = normalize(V + L);
        float attenuation = 1.0f; // No attenuation
        float3 radiance = finalLightColor * attenuation;
        
        float dotNV = max(dot(N, V), 0.0f); // Dot ( Normal, View )
        float dotNL = max(dot(N, L), 0.0f); // Dot ( Normal, OutgoingLight )
        float dotNH = max(dot(N, H), 0.0f); // Dot ( Normal, Halfway )
        
        //Cook-Torrance BRDF
        float normalDistribution = NormalDistributionGGX(dotNH, roughness);
        float geometry = GeometrySmith(dotNV, dotNL, roughness);
        float3 fresnel = FresnelSchlick(max(dot(H, V), 0.0f), F0);

        float3 numerator = normalDistribution * geometry * fresnel;
        float denominator = 4 * dotNV * dotNL;
        float3 specularBRDF = numerator / max(denominator, 0.001f);

        //Energy Conservation
        float3 kS = fresnel;
        float3 kD = 1.0f - kS;
        
        
        //No Diffuse for Conductor
        kD *= 1.0f - metallic;
        
        Lo += (kD * albedo / PI + specularBRDF) * radiance * dotNL;
    }

    
    // AMBIENT
    float3 kS = FresnelSchlickRoughness(max(dot(N, V), 0.0f), F0, roughness);
    float3 kD = 1.0f - kS;
    float3 irradiance = bToggleIrradianceTexture ? IrradianceTexture.Sample(DefaultSampler, N).rgb  : 1.0f;
    float3 diffuse = irradiance * albedo;
    float3 ambient =  kD * diffuse;

    float3 finalColor = emissive + Lo + ambient;

    float brightness = dot(finalColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));

    PS_OUTPUT output;

    output.MainColor = float4(finalColor, 1.0f);

    if(brightness > 1.0f)
        output.BrightColor = output.MainColor;
    else
        output.BrightColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

    return output;
}