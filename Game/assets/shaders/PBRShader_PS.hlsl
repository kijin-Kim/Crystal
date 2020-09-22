

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION;
    float4 WorldLightPosition : POSITION1;
    float4 WorldCameraPosition : POSITION2;
    float3 WorldNormal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

Texture2D textures[2] : register(t0);
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


float4 psMain(VS_OUTPUT input) : SV_TARGET
{
    //Current we have only one directional light
    float3 lightColor = 1.0f;
    float3 albedo = pow(textures[0].Sample(normalSampler, input.TexCoord).rgb, float3(2.2f, 2.2f,2.2f));
    float roughness = textures[1].Sample(normalSampler, input.TexCoord).r;
    float metallic = textures[2].Sample(normalSampler, input.TexCoord).r;
    int lightCount = 1;
    ////////////////

    float3 N = normalize(input.WorldNormal);
    float3 V = normalize(input.WorldCameraPosition - input.WorldPosition).xyz;

    float3 F0 = 0.04f;
    F0 = lerp(F0, albedo, metallic);

    float3 Lo = 0.0f;
    for(int i= 0; i < lightCount; i++)
    {
        // L()
        float3 L = normalize(input.WorldLightPosition - input.WorldPosition).xyz;
        float3 H = normalize(V + L);
        float distance = length(input.WorldLightPosition - input.WorldCameraPosition);
        float attenuation = 1.0f / (distance * distance);
        //float attenuation = 1.0f / dot(float3(1.0f, 0.0f, 1.0f), float3(1.0f, distance, distance*distance));

        attenuation = 1.0f; // TEMP
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
        float3 diffuseEnergy = float3(1.0f, 1.0f, 1.0f) - fresnel;
        //No Diffuse for Conductor
        diffuseEnergy = lerp(float3(1.0f,1.0f, 1.0f) - specularEnergy, float3(0.0f, 0.0f, 0.0f), metallic);
        

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