#include "LightHelper.hlsli"
static const float PI = 3.1415926f; // ÉŒ
struct PS_IN
{
	float4 pos : SV_POSITION0;
	float3 normal : NORMAL0;
	float2 tex : TEXCOORD0;
	float4 tangentW : TANGENT;
	float4 worldPos : POSITION0;
};

cbuffer Camera : register(b0)
{
	//Camera Position
	float4 eyePos;
}

//SunLight
cbuffer DirLight:register(b1)
{
	float4 lightDiffuse;
	float3 lightPos;
	float lightIntensity;
}

//cbuffer PointLights : register(b2)
//{
//	PointLight pointLight[2];
//}

//todo:ä¬ã´åıí«â¡ÅI
Texture2D albedoTex : register(t0);
Texture2D normalMap : register(t1);
Texture2D metallicSmoothMap : register(t2);
SamplerState mySampler : register(s0);


float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = saturate(dot(N, H));
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
	denom = PI * denom * denom;

	return nom / max(denom, 0.0000001);
}

// Schlick
float3 SchlickFresnel(float3 F0, float3 H, float3 V)
{
	float cosIncidentAngle = saturate(dot(H, V));

	return F0 + (1.0f - F0) * pow(1 - cosIncidentAngle, 5);
}

// 
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
   
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

// Cook-Torrance 
float3 CookTorranceSpecular(float3 N, float3 L, float3 V, float roughness, float3 F0, out float3 Ks)
{
	float3 h = normalize(V + L);

	float D = DistributionGGX(N, h, roughness);
	float3 F = SchlickFresnel(F0, h, V);
	Ks = F;
	float G = GeometrySmith(N, V, L, roughness);
	float NdotV = max(dot(N, V), 0.0f);
	float NdotL = max(dot(N, L), 0.0f);
	return (D * F * G) / (4 * max(NdotV * NdotL, 0.01f));
}

// Lambert diffuse
float3 LambertDiffuse(float3 Ks, float3 albedo, float metallic)
{
	float3 Kd = (float3(1.0f, 1.0f, 1.0f) - Ks) * (1 - metallic);
	return (Kd * albedo / PI);
}

// Environment Light PBR
float3 EnvironmentPBR(float lightIntensity, float3 lightColor, float3 toLight, float3 normal, float3 toEyeW_Unit, float roughness, float metallic, float3 albedo, float shadowAmount)
{
	float3 f0 = lerp(F0_NON_METAL.rrr, albedo.rgb, metallic);
	float3 kS = float3(0.f, 0.f, 0.f);
	float3 specularBRDF = CookTorranceSpecular(normal, toLight, toEyeW_Unit, roughness, f0, kS);
	float3 diffuseBRDF = LambertDiffuse(kS, albedo, metallic);
	return (diffuseBRDF + specularBRDF) * lightIntensity * lightColor.rgb * shadowAmount;
}

// Directional Light PBR
float3 DirectionLightPBR(float lightIntensity, float3 lightColor, float3 toLight, float3 normal, float3 toEyeW_Unit, float roughness, float metallic, float3 albedo, float shadowAmount)
{
	float3 f0 = lerp(F0_NON_METAL.rrr, albedo.rgb, metallic);
	float3 kS = float3(0.f, 0.f, 0.f);
	float3 specularBRDF = CookTorranceSpecular(normal, toLight, toEyeW_Unit, roughness, f0, kS);
	float3 diffuseBRDF = LambertDiffuse(kS, albedo, metallic);

	float NdotL = max(dot(normal, toLight), 0.0);
	return (diffuseBRDF + specularBRDF) * lightIntensity * lightColor.rgb * shadowAmount;
}

float3 PointLightPBR(PointLight pointLight, float3 pos, float3 normal, float3 toEyeW_Unit)
{
	float3 color = float3(0, 0, 0);
	if (pointLight.isEnable == 0.f)
	{
		return color;
	}
	else
	{
		//calculate vertex to pointLight vector
		float3 toLightVec = pointLight.position - pos;
		float3 V = normalize(toLightVec);
		float3 toLightLen = length(toLightVec);
		//calculate attenuation
		float3 N = normalize(normal);
		float dotNV = saturate(dot(N, V));
		float3 attenuation = saturate(1.0f - toLightLen / pointLight.range);
		attenuation *= pointLight.att.x;
		attenuation = pow(attenuation, 2.f);

		color = pointLight.diffuse.rgb * dotNV * attenuation;
		return color;
	}
	
}

float4 main(PS_IN pin) : SV_TARGET
{
	float4 albedoColor = albedoTex.Sample(mySampler, pin.tex);
	float metallic = metallicSmoothMap.Sample(mySampler, pin.tex).r;
	float smooth = metallicSmoothMap.Sample(mySampler, pin.tex).a;
	float roughness = 1.0f - smooth;

	float3 normalW = normalize(pin.normal);
	float4 tangentW = normalize(pin.tangentW);

	float3 normalMapSample = normalMap.Sample(mySampler, pin.tex).xyz;
	float3 N = NormalSampleToWorldSpace(normalMapSample, normalW, tangentW);
	float3 V = normalize(eyePos - pin.worldPos).xyz;
	float3 L = normalize(lightPos.xyz - pin.worldPos.xyz);

	//Environment Light
	//float3  environmentColor = EnvironmentPBR(lightIntensity, lightDiffuse.rgb, L, N, V, roughness, metallic, albedoColor.xyz, 1.0f);
	float3 environmentColor = { 0, 0, 0 };

	//Dir Light
	float3 directionalColor = DirectionLightPBR(lightIntensity, lightDiffuse.rgb, L, N, V, roughness, metallic, albedoColor.xyz, 1.0f);

	//PointLight process
	//float3 pointLightColor = float3(0, 0, 0);
	//for (int i = 0; i < 2; i++)
	//	pointLightColor += PointLightPBR(pointLight[i], pin.worldPos.xyz, N, V);


	//float3 color = albedoColor.rgb * (pointLightColor + pbrColor);
	float3 color = albedoColor.rgb * (environmentColor + directionalColor);
	return float4(color, 1.0f);


}
