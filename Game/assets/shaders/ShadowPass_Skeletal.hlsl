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

cbuffer PerFrameData : register(b0)
{
    float4x4 ViewProjection : packoffset(c0);
}

cbuffer PerObjectData : register(b1)
{
    float4x4 World;
    float4x4 Bones[300];
}



float4 vsMain(VS_INPUT input) : SV_Position
{
    float4x4 boneTransform = Bones[input.BoneIDs[0]] * input.BoneWeights[0];
    boneTransform += Bones[input.BoneIDs[1]] * input.BoneWeights[1];
    boneTransform += Bones[input.BoneIDs[2]] * input.BoneWeights[2];
    boneTransform += Bones[input.BoneIDs[3]] * (1.0f - (input.BoneWeights[2] + input.BoneWeights[1] + input.BoneWeights[0]));
    
    
    float3 localPosition = mul(float4(input.Position, 1.0f), boneTransform);
    float4 position = mul(mul(float4(localPosition, 1.0f), World), ViewProjection);

    return position;
}
