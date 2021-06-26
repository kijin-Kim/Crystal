//this code is based on tutorial 
//https://learnopengl.com/PBR/Lighting, 
//https://learnopengl.com/PBR/IBL/Diffuse-irradiance


struct VS_INPUT
{
    float2 Position : POSITION;    
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
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
    output.Position = float4(input.Position, 1.0f, 1.0f);
    output.TexCoord = (output.Position + 1.0f) / 2.0f;
	output.TexCoord.y = -output.TexCoord.y;
    
    return output;
}


Texture2D AlbedoBuffer : register(t0);
Texture2D RoughnessMetallicAOBuffer : register(t1);
Texture2D EmissiveBuffer : register(t2);
Texture2D WorldNormalBuffer : register(t3);
Texture2D IrradianceBuffer : register(t4);
Texture2D WorldPositionBuffer : register(t5);
Texture2D ShadowMap : register(t6);


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

float CalculateShadow(float4 lightSpacePosition)
{
    float3 projected = lightSpacePosition.xyz / lightSpacePosition.w; // perspective division
    float2 texCoord = projected.xy * 0.5f + 0.5f; // [-1 , 1] to [0 , 1] range excepct z
    texCoord.y = -texCoord.y;

    float closest = ShadowMap.Sample(DefaultSampler, texCoord).r;
    float current = projected.z;

    return current > closest ? 1.0f : 0.0f;
}




PS_OUTPUT psMain(PS_INPUT input)
{
    //Current we have only one directional light
    float3 albedo = AlbedoBuffer.Sample(DefaultSampler, input.TexCoord).rgb;
    float3 roughnessMetallicAO =  RoughnessMetallicAOBuffer.Sample(DefaultSampler, input.TexCoord).rgb;
    float3 emissive = EmissiveBuffer.Sample(DefaultSampler, input.TexCoord).rgb;
    float3 irradiance = IrradianceBuffer.Sample(DefaultSampler, input.TexCoord).rgb;
    float3 worldNormal = WorldNormalBuffer.Sample(DefaultSampler, input.TexCoord).rgb;
    float3 worldPosition = WorldPositionBuffer.Sample(DefaultSampler, input.TexCoord).rgb;

    
    /*�� ����*/
    float3 V = normalize(WorldCameraPosition.xyz - worldPosition);
    
    /*Base Reflectivity*/
    float3 F0 = 0.04f;
    F0 = lerp(F0, albedo, roughnessMetallicAO.g);

    
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
        
        float dotNV = max(dot(worldNormal, V), 0.0f); // Dot ( Normal, View )
        float dotNL = max(dot(worldNormal, L), 0.0f); // Dot ( Normal, OutgoingLight )
        float dotNH = max(dot(worldNormal, H), 0.0f); // Dot ( Normal, Halfway )
        
        //Cook-Torrance BRDF
        float normalDistribution = NormalDistributionGGX(dotNH, roughnessMetallicAO.r);
        float geometry = GeometrySmith(dotNV, dotNL, roughnessMetallicAO.r);
        float3 fresnel = FresnelSchlick(max(dot(H, V), 0.0f), F0);

        float3 numerator = normalDistribution * geometry * fresnel;
        float denominator = 4 * dotNV * dotNL;
        float3 specularBRDF = numerator / max(denominator, 0.001f);

        //Energy Conservation
        float3 kS = fresnel;
        float3 kD = 1.0f - kS;
        
        
        //No Diffuse for Conductor
        kD *= 1.0f - roughnessMetallicAO.g;
        
        Lo += (kD * albedo / PI + specularBRDF) * radiance * dotNL;
    }

    
    // AMBIENT
    float3 kS = FresnelSchlickRoughness(max(dot(worldNormal, V), 0.0f), F0, roughnessMetallicAO.r);
    float3 kD = 1.0f - kS;
    float3 diffuse = irradiance * albedo;
    float3 ambient =  kD * diffuse;


    float shadow = CalculateShadow(mul(float4(worldPosition, 1.0f), LightViewProjection));
    float3 finalColor = emissive + Lo * (1.0 - shadow) + ambient;

    float brightness = dot(finalColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));

    PS_OUTPUT output = (PS_OUTPUT)0;

    output.MainColor = float4(finalColor, 1.0f);
    if(brightness > 1.0f)
        output.BrightColor = output.MainColor;
    else
        output.BrightColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

    return output;
}
