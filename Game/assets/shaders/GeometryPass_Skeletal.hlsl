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
    float3 WorldTangent : WORLD_TANGENT;
    float3 WorldBiTangent : WORLD_BITANGENT;
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
    float4x4 ViewProjection : packoffset(c0);
}

cbuffer PerObjectData : register(b1)
{
    float4x4 World;
    float4x4 Bones[300];
}

cbuffer PerExecute : register(b2)
{
    float3 AlbedoColor;
    bool bToggleAlbedoTexture;
    bool bToggleMetallicTexture;
    bool bToggleRoughnessTexture;
    bool bToggleNormalTexture;
    bool bToggleIrradianceTexture;
    bool bToggleEmissiveTexture;
    float RoughnessConstant;
    float MetallicConstant;
    float3 EmissiveColor;
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
    
    output.WorldTangent = mul(input.Tangent, (float3x3) World);
    output.WorldBiTangent = mul(input.BiTangent, (float3x3) World);

    return output;
}


#define TEXTURE_TYPE_ALBEDO 0
#define TEXTURE_TYPE_METALLIC 1
#define TEXTURE_TYPE_ROUGHNESS 2
#define TEXTURE_TYPE_NORMAL 3
#define TEXTURE_TYPE_EMISSIVE 4

TextureCube IrradianceTexture : register(t0);
Texture2D Textures[5] : register(t1);

SamplerState DefaultSampler : register(s0);


PS_OUTPUT psMain(PS_INPUT input) : SV_TARGET
{
    float3 albedo = bToggleAlbedoTexture ? 
    pow(Textures[TEXTURE_TYPE_ALBEDO].Sample(DefaultSampler, input.TexCoord).rgb, float3(2.2f, 2.2f, 2.2f)) : AlbedoColor.rgb;
    float roughness = bToggleRoughnessTexture ? 
    Textures[TEXTURE_TYPE_ROUGHNESS].Sample(DefaultSampler, input.TexCoord).r : RoughnessConstant;
    float metallic = bToggleMetallicTexture ? 
    Textures[TEXTURE_TYPE_METALLIC].Sample(DefaultSampler, input.TexCoord).r : MetallicConstant;
    float3 emissive = bToggleEmissiveTexture ? 
    Textures[TEXTURE_TYPE_EMISSIVE].Sample(DefaultSampler, input.TexCoord).rgb : EmissiveColor;
    
    
    float3 N = bToggleNormalTexture ? mul(normalize(Textures[TEXTURE_TYPE_NORMAL].Sample(DefaultSampler, input.TexCoord).rgb * 2.0f - 1.0f),
    float3x3(normalize(input.WorldTangent), normalize(input.WorldBiTangent), normalize(input.WorldNormal))) : input.WorldNormal;
    N = normalize(N);

    float3 irradiance = bToggleIrradianceTexture ? IrradianceTexture.Sample(DefaultSampler, N).rgb : 1.0f;

    PS_OUTPUT output = (PS_OUTPUT)0;

    output.AlbedoBuffer = float4(albedo, 1.0f);
    output.RoughnessMetallicAOBuffer = float4(roughness, metallic, 1.0f, 1.0f);
    output.EmissiveBuffer = float4(emissive, 1.0f);
    output.WorldNormalBuffer = float4(N, 1.0f);
    output.IrradianceBuffer = float4(irradiance, 1.0f);
    output.WorldPositionBuffer = input.WorldPosition;
    
    return output;
}