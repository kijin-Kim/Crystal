

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
    uint MaterialIndex : MATERIAL_INDEX;
    float3 AlbedoColor : ALBEDO_COLOR;
    float3 EmissiveColor : EMISSIVE_COLOR;
    float RoughnessConstant : ROUGHNESS_CONSTANT;
    float MetallicConstant : METALLIC_CONSTANT;

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
    nointerpolation uint MaterialIndex : MATERIAL_INDEX;
    nointerpolation float3 AlbedoColor : ALBEDO_COLOR;
    nointerpolation float3 EmissiveColor : EMISSIVE_COLOR;
    nointerpolation float RoughnessConstant : ROUGHNESS_CONSTANT;
    nointerpolation float MetallicConstant : METALLIC_CONSTANT;

    nointerpolation bool bToggleAlbedoTexture : TOGGLE_ALBEDO_TEXTURE;
    nointerpolation bool bToggleMetallicTexture : TOGGLE_METALLIC_TEXTURE; 
    nointerpolation bool bToggleRoughnessTexture : TOGGLE_ROUGHNESS_TEXTURE;
    nointerpolation bool bToggleNormalTexture : TOGGLE_NORMAL_TEXTURE;
    nointerpolation bool bToggleIrradianceTexture : TOGGLE_IRRADIANCE_TEXTURE;
    nointerpolation bool bToggleEmissivetexture : TOGGLE_EMISSIVE_TEXTURE;

};

struct PS_OUTPUT
{
    float4 AlbedoBuffer : SV_Target0;
    float4 RoughnessMetallicAOBuffer : SV_Target1;
    float4 EmissiveBuffer : SV_Target2;
    float4 WorldNormalBuffer : SV_Target3;
    float4 IrradianceBuffer : SV_Target4;
    float4 WorldPositionBuffer : SV_Target5;
};

cbuffer PerFrameData : register(b0)
{
    float4x4 ViewProjection;
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


    output.MaterialIndex = input.MaterialIndex;
    output.AlbedoColor = input.AlbedoColor;
    output.EmissiveColor = input.EmissiveColor;
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



#define MAX_PER_INSTANCE_TEXTURE_COUNT 10

#define TEXTURE_TYPE_ALBEDO 0
#define TEXTURE_TYPE_METALLIC 1
#define TEXTURE_TYPE_ROUGHNESS 2
#define TEXTURE_TYPE_NORMAL 3
#define TEXTURE_TYPE_EMISSIVE 4

TextureCube IrradianceTexture : register(t0);
Texture2D Textures[MAX_PER_INSTANCE_TEXTURE_COUNT][5] : register(t1);
StructuredBuffer<float4x4> Bones[300 * 100];
SamplerState DefaultSampler : register(s0);


PS_OUTPUT psMain(PS_INPUT input) : SV_TARGET
{
    
    float3 albedo = input.bToggleAlbedoTexture ? 
    pow(Textures[input.MaterialIndex][TEXTURE_TYPE_ALBEDO].Sample(DefaultSampler, input.TexCoord).rgb, float3(2.2f, 2.2f, 2.2f)) : input.AlbedoColor.rgb;
    float roughness = input.bToggleRoughnessTexture ? 
    Textures[input.MaterialIndex][TEXTURE_TYPE_ROUGHNESS].Sample(DefaultSampler, input.TexCoord).r : input.RoughnessConstant;
    float metallic = input.bToggleMetallicTexture ? 
    Textures[input.MaterialIndex][TEXTURE_TYPE_METALLIC].Sample(DefaultSampler, input.TexCoord).r : input.MetallicConstant;
    float3 emissive = input.bToggleEmissivetexture ? 
    Textures[input.MaterialIndex][TEXTURE_TYPE_EMISSIVE].Sample(DefaultSampler, input.TexCoord).rgb : input.EmissiveColor;
    
    
    float3 N = input.bToggleNormalTexture ? mul(normalize(Textures[input.MaterialIndex][TEXTURE_TYPE_NORMAL].Sample(DefaultSampler, input.TexCoord).rgb * 2.0f - 1.0f),
    float3x3(normalize(input.WorldTangent), normalize(input.WorldBiTangent), normalize(input.WorldNormal))) : input.WorldNormal;
    N = normalize(N);

    float3 irradiance = input.bToggleIrradianceTexture ? IrradianceTexture.Sample(DefaultSampler, N).rgb : 1.0f;

    PS_OUTPUT output = (PS_OUTPUT)0;

    output.AlbedoBuffer = float4(albedo, 1.0f);
    output.RoughnessMetallicAOBuffer = float4(roughness, metallic, 1.0f, 1.0f);
    output.EmissiveBuffer = float4(emissive, 1.0f);
    output.WorldNormalBuffer = float4(N, 1.0f);
    output.IrradianceBuffer = float4(irradiance, 1.0f);
    output.WorldPositionBuffer = input.WorldPosition;
    
    return output;
}