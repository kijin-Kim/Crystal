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




#define LT_Directional 0
#define LT_Point 1

struct Light
{
    int LightType;
    float Intensity;
    float AttenuationRadius;
    int bCastShadow;

    float3 Direction;
    float padding2;

    float3 Position;
    float padding3;

    float3 Color;
    float padding4;
};

cbuffer PerFrameData : register(b0)
{
    float4x4 ViewProjection : packoffset(c0);
    float4x4 LightViewProjection : packoffset(c4);
    float4 WorldCameraPosition : packoffset(c8);
    Light Lights[100] : packoffset(c9);
    int LightCount : packoffset(c409);
}



cbuffer PerObjectData : register(b1)
{
    float4x4 World;
}

cbuffer PerDrawData : register(b2)
{
    float3 AlbedoColor;
    float padding0;
    float3 EmissiveColor;
    float padding2;
    float RoughnessConstant;
    float MetallicConstant;
    float2 TexCoordMultiplier;

    bool bShouldLit;
    float Opacity;
    float OpacityMultiplier;
    bool bToggleAlbedoTexture;
    bool bToggleMetallicTexture;
    bool bToggleRoughnessTexture;
    bool bToggleNormalTexture;
    bool bToggleIrradianceTexture;
    bool bToggleEmissiveTexture;
    bool bToggleOpacityTexture;
}

PS_INPUT vsMain(VS_INPUT input)
{
    PS_INPUT output;


    output.Position = mul(mul(float4(input.Position, 1.0f), World), ViewProjection);
    output.WorldPosition = mul(float4(input.Position, 1.0f), World);


    input.TexCoord.x *= TexCoordMultiplier.x;
    input.TexCoord.y *= TexCoordMultiplier.y;

    
    output.TexCoord = input.TexCoord;
    output.WorldNormal = mul(input.Normal, (float3x3) World);
    
    output.WorldTangent = mul(input.Tangent, (float3x3) World);
    output.WorldBiTangent = mul(input.BiTangent, (float3x3) World);

    return output;
}







#define TEXTURE_TYPE_ALBEDO 0
#define TEXTURE_TYPE_METALLIC 1
#define TEXTURE_TYPE_ROUGHNESS 2
#define TEXTURE_TYPE_NORMAL 3
#define TEXTURE_TYPE_EMISSIVE 4
#define TEXTURE_TYPE_OPACITY 5

TextureCube IrradianceTexture : register(t0);
Texture2D ShadowMap : register(t1);
Texture2D Textures[6] : register(t2);



SamplerState DefaultSampler : register(s0);
SamplerState ShadowMapSampler : register(s1);


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

float CalculateShadow(float4 lightSpacePosition)
{
    float3 projected = lightSpacePosition.xyz / lightSpacePosition.w; // perspective division
    if(projected.z > 1.0f)
    {
        return 0.0f;
    }

    float2 texCoord = float2(projected.x * 0.5f + 0.5f, -projected.y * 0.5f + 0.5f); // [-1 , 1] to [0 , 1] range excepct z


    float shadow = 0.0f;

    uint shadowMapWidth = 0;
    uint shadowMapHeight = 0;
    ShadowMap.GetDimensions(shadowMapWidth, shadowMapHeight);

    float2 texelSize = float2(1.0f / float(shadowMapWidth), 1.0f / float(shadowMapHeight));
    float bias = 0.0008f;

    for(int x = -1; x <= 1; ++x) // PCF
    {
        for(int y = -1; y <= 1; ++y)
        {
            float closest = ShadowMap.SampleLevel(ShadowMapSampler, texCoord + float2(x,-y) * texelSize, 0).r;
            float current = projected.z;
            shadow += current > closest + bias ? 1.0f : 0.0f;
        }
    }
    shadow /= 9.0f;

    return shadow;
}



float4 psMain(PS_INPUT input) : SV_TARGET
{
    //Current we have only one directional light
    float3 emissive = bToggleEmissiveTexture ? Textures[TEXTURE_TYPE_EMISSIVE].Sample(DefaultSampler, input.TexCoord).rgb : EmissiveColor;
    float opacity = bToggleOpacityTexture ? Textures[TEXTURE_TYPE_OPACITY].Sample(DefaultSampler, input.TexCoord).r : Opacity;
    opacity *= OpacityMultiplier;
    if(!bShouldLit)
    {
        return float4(emissive, opacity);
    }
    


    float3 albedo = bToggleAlbedoTexture ? pow(Textures[TEXTURE_TYPE_ALBEDO].Sample(DefaultSampler, input.TexCoord).rgb, float3(2.2f, 2.2f, 2.2f)) : AlbedoColor.rgb;
    float roughness = bToggleRoughnessTexture ? 
    Textures[TEXTURE_TYPE_ROUGHNESS].Sample(DefaultSampler, input.TexCoord).r : RoughnessConstant;
    float metallic = bToggleMetallicTexture ? Textures[TEXTURE_TYPE_METALLIC].Sample(DefaultSampler, input.TexCoord).r : MetallicConstant;
    
    
    
    /*�븻*/
    float3 N = bToggleNormalTexture ? mul(normalize(Textures[TEXTURE_TYPE_NORMAL].Sample(DefaultSampler, input.TexCoord).rgb * 2.0f - 1.0f),
    float3x3(normalize(input.WorldTangent), normalize(input.WorldBiTangent), normalize(input.WorldNormal))) : input.WorldNormal;
    N = normalize(N);

    float3 irradiance = bToggleIrradianceTexture ? IrradianceTexture.Sample(DefaultSampler, N).rgb : 1.0f;

    
    /*�� ����*/
    float3 V = normalize(WorldCameraPosition.xyz - input.WorldPosition.xyz);
    
    /*Base Reflectivity*/
    float3 F0 = 0.04f;
    F0 = lerp(F0, albedo, metallic);

    float3 Lo = 0.0f;
    [unroll(30)] for (int i = 0; i < LightCount; i++) 
    {  
        float3 finalLightColor = Lights[i].Color * Lights[i].Intensity;

        // L()
        //float3 L = normalize(Lights[i].WorldPosition.xyz - worldPosition);
        float attenuation = 0.0f; // No attenuation
        float3 L  = 0.0f;
        if(Lights[i].LightType == LT_Directional)
        {
            L = -Lights[i].Direction;
            attenuation = 1.0f;
        } 
        else if(Lights[i].LightType == LT_Point)
        {
            float3 diff = Lights[i].Position - input.WorldPosition.xyz;
            L = normalize(diff);
            float r = saturate(length(diff) / Lights[i].AttenuationRadius);
            attenuation = saturate(1.0 - r);
        }        
        float3 H = normalize(V + L);


        
        float dotNV = max(dot(input.WorldNormal, V), 0.0f); // Dot ( Normal, View )
        float dotNL = max(dot(input.WorldNormal, L), 0.0f); // Dot ( Normal, OutgoingLight )
        float dotNH = max(dot(input.WorldNormal, H), 0.0f); // Dot ( Normal, Halfway )
        
        //Cook-Torrance BRDF
        float3 fresnel = FresnelSchlick(max(dot(H, V), 0.0f), F0);
        float normalDistribution = NormalDistributionGGX(dotNH, roughness);
        float geometry = GeometrySmith(dotNV, dotNL, roughness);
        

        float3 numerator = normalDistribution * geometry * fresnel;
        float denominator = 4 * dotNV * dotNL;
        float3 specularBRDF = numerator / max(denominator, 0.001f);

        //Energy Conservation
        float3 kS = fresnel;
        float3 kD = 1.0f - kS;
        
        
        //No Diffuse for Conductor
        kD *= 1.0f - metallic;
        
        float3 radiance = finalLightColor * attenuation;

        float3 finalL = (kD * albedo / PI + specularBRDF) * radiance * dotNL;
        if(Lights[i].bCastShadow)
        {
            float shadow = CalculateShadow(mul(float4(input.WorldPosition.xyz, 1.0f), LightViewProjection));
            finalL *= (1.0f - shadow);
        }
        Lo += finalL;



    }


    // AMBIENT
    float3 kS = FresnelSchlickRoughness(max(dot(input.WorldNormal, V), 0.0f), F0, roughness);
    float3 kD = 1.0f - kS;
    float3 diffuse = irradiance * albedo;
    float3 ambient =  kD * diffuse;


    float3 finalColor = emissive + Lo + ambient;


    return float4(finalColor, opacity);
}