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

    float4 MatRow0 : MATROW0;
    float4 MatRow1 : MATROW1;
    float4 MatRow2 : MATROW2;
    float4 MatRow3 : MATROW3;

    // perinstance data for pixel shader
    float3 AlbedoColor : ALBEDO_COLOR;
    float3 EmissiveColor : EMISSIVE_COLOR;
    float RoughnessConstant : ROUGHNESS_CONSTANT;
    float MetallicConstant : METALLIC_CONSTANT;

    bool bShouldLit : SHOULD_LIT;
    bool bToggleAlbedoTexture : TOGGLE_ALBEDO_TEXTURE;
    bool bToggleMetallicTexture : TOGGLE_METALLIC_TEXTURE;
    bool bToggleRoughnessTexture : TOGGLE_ROUGHNESS_TEXTURE;
    bool bToggleNormalTexture : TOGGLE_NORMAL_TEXTURE;
    bool bToggleIrradianceTexture : TOGGLE_IRRADIANCE_TEXTURE;
    bool bToggleEmissivetexture : TOGGLE_EMISSIVE_TEXTURE;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION;
    float3 WorldNormal : WORLD_NORMAL;
    float2 TexCoord : TEXCOORD;
    float3 WorldTangent : WORLD_TANGENT;
    float3 WorldBiTangent : WORLD_BITANGENT;
    
    // perinstance data for pixel shader
    nointerpolation float3 AlbedoColor : ALBEDO_COLOR;
    nointerpolation float3 EmissiveColor : EMISSIVE_COLOR;
    nointerpolation float RoughnessConstant : ROUGHNESS_CONSTANT;
    nointerpolation float MetallicConstant : METALLIC_CONSTANT;

    nointerpolation bool bShouldLit : SHOULD_LIT;
    nointerpolation bool bToggleAlbedoTexture : TOGGLE_ALBEDO_TEXTURE;
    nointerpolation bool bToggleMetallicTexture : TOGGLE_METALLIC_TEXTURE; 
    nointerpolation bool bToggleRoughnessTexture : TOGGLE_ROUGHNESS_TEXTURE;
    nointerpolation bool bToggleNormalTexture : TOGGLE_NORMAL_TEXTURE;
    nointerpolation bool bToggleIrradianceTexture : TOGGLE_IRRADIANCE_TEXTURE;
    nointerpolation bool bToggleEmissivetexture : TOGGLE_EMISSIVE_TEXTURE;

};

struct PS_OUTPUT
{
    float4 MainColor : SV_Target0;
    float4 BrightColor : SV_Target1;
};


struct Light
{
    float3 Direction;
    float padding0;
    float4 Color;
    float Intensity;
    float3 padding1;
};


cbuffer PerFrameData : register(b0)
{
    float4x4 ViewProjection : packoffset(c0);
    float4x4 LightViewProjection : packoffset(c4);
    float4 WorldCameraPosition : packoffset(c8);
    Light Lights[100] : packoffset(c9);
    int LightCount : packoffset(c309);
}

PS_INPUT vsMain(VS_INPUT input)
{
    PS_INPUT output;

    float4x4 World = float4x4(input.MatRow0, input.MatRow1, input.MatRow2 ,input.MatRow3);

    output.Position = mul(mul(float4(input.Position, 1.0f), World), ViewProjection);
    output.WorldPosition = mul(float4(input.Position, 1.0f), World);

    output.TexCoord = input.TexCoord;
    output.WorldNormal = mul(input.Normal, (float3x3) World);
    
    output.WorldTangent = mul(input.Tangent, (float3x3) World);
    output.WorldBiTangent = mul(input.BiTangent, (float3x3) World);


    output.AlbedoColor = input.AlbedoColor;
    output.EmissiveColor = input.EmissiveColor;
    output.bShouldLit = input.bShouldLit;
    output.bToggleAlbedoTexture = input.bToggleAlbedoTexture;
    output.bToggleMetallicTexture = input.bToggleMetallicTexture;
    output.bToggleRoughnessTexture = input.bToggleRoughnessTexture;
    output.bToggleNormalTexture  = input.bToggleNormalTexture;
    output.bToggleIrradianceTexture  = input.bToggleIrradianceTexture;
    output.bToggleEmissivetexture  = input.bToggleEmissivetexture;
    output.RoughnessConstant = input.RoughnessConstant;
    output.MetallicConstant  = input.MetallicConstant;

    return output;
}







#define TEXTURE_TYPE_ALBEDO 0
#define TEXTURE_TYPE_METALLIC 1
#define TEXTURE_TYPE_ROUGHNESS 2
#define TEXTURE_TYPE_NORMAL 3
#define TEXTURE_TYPE_EMISSIVE 4

TextureCube IrradianceTexture : register(t0);
Texture2D ShadowMap : register(t1);
Texture2D Textures[5] : register(t2);



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
    float bias = 0.0001f;

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



PS_OUTPUT psMain(PS_INPUT input)
{
    //Current we have only one directional light
    float3 emissive = input.bToggleEmissivetexture ? Textures[TEXTURE_TYPE_EMISSIVE].Sample(DefaultSampler, input.TexCoord).rgb : input.EmissiveColor;
    if(!input.bShouldLit)
    {
        float3 finalColor = emissive;
        float brightness = dot(finalColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));

        PS_OUTPUT output = (PS_OUTPUT)0;

        output.MainColor = float4(finalColor, 1.0f);

        if(brightness > 1.0f)
            output.BrightColor = output.MainColor;
        else
            output.BrightColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

        return output;
    }

    float3 albedo = input.bToggleAlbedoTexture ? pow(Textures[TEXTURE_TYPE_ALBEDO].Sample(DefaultSampler, input.TexCoord).rgb, float3(2.2f, 2.2f, 2.2f)) : input.AlbedoColor.rgb;
    float roughness = input.bToggleRoughnessTexture ? 
    Textures[TEXTURE_TYPE_ROUGHNESS].Sample(DefaultSampler, input.TexCoord).r : input.RoughnessConstant;
    float metallic = input.bToggleMetallicTexture ? 
    Textures[TEXTURE_TYPE_METALLIC].Sample(DefaultSampler, input.TexCoord).r : input.MetallicConstant;
    
    
    /*�븻*/
    float3 N = input.bToggleNormalTexture ? mul(normalize(Textures[TEXTURE_TYPE_NORMAL].Sample(DefaultSampler, input.TexCoord).rgb * 2.0f - 1.0f),
    float3x3(normalize(input.WorldTangent), normalize(input.WorldBiTangent), normalize(input.WorldNormal))) : input.WorldNormal;
    N = normalize(N);

    float3 irradiance = input.bToggleIrradianceTexture ? IrradianceTexture.Sample(DefaultSampler, N).rgb : 1.0f;

    
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
        float3 L = -Lights[i].Direction;
        float3 H = normalize(V + L);
        float attenuation = 1.0f; // No attenuation
        float3 radiance = finalLightColor * attenuation;
        
        float dotNV = max(dot(input.WorldNormal, V), 0.0f); // Dot ( Normal, View )
        float dotNL = max(dot(input.WorldNormal, L), 0.0f); // Dot ( Normal, OutgoingLight )
        float dotNH = max(dot(input.WorldNormal, H), 0.0f); // Dot ( Normal, Halfway )
        
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
    float3 kS = FresnelSchlickRoughness(max(dot(input.WorldNormal, V), 0.0f), F0, roughness);
    float3 kD = 1.0f - kS;
    float3 diffuse = irradiance * albedo;
    float3 ambient =  kD * diffuse;


    float shadow = CalculateShadow(mul(input.WorldPosition, LightViewProjection));
    float3 finalColor = emissive + Lo * (1.0f - shadow) + ambient;

    float brightness = dot(finalColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));

    PS_OUTPUT output = (PS_OUTPUT)0;

    output.MainColor = float4(finalColor, 1.0f);
    if(brightness > 1.0f)
        output.BrightColor = output.MainColor;
    else
        output.BrightColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

    return output;
}