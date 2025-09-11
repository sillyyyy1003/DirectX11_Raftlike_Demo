#include "LightHelper.hlsli"
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
cbuffer DayLight:register(b1)
{
	float4 lightAmbient;
	float4 lightDiffuse;
	float3 lightDir;
	float lightIntensity;
}


//todo:ä¬ã´åıí«â¡ÅI
Texture2D albedoTex : register(t0);
Texture2D normalMap : register(t1);
Texture2D metallicSmoothMap : register(t2);
SamplerState mySampler : register(s0);


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
	float3 L = normalize(-lightDir); //Directional light direction

	float3 ambient = albedoColor.rgb * lightAmbient.rgb * lerp(0.2, 0.05, metallic) * min(lightIntensity, 4.); // ambient contribution & avoid too much light intensity

	//Dir Light
	float3 directionalColor = DirectionLightPBR(lightIntensity, lightDiffuse.rgb, L, N, V, roughness, metallic, albedoColor.xyz, 1.0f);
	float3 pbrColor = 0;
	pbrColor += directionalColor;

	float3 totalColor = pbrColor + ambient;
	return float4(totalColor, 1.0f);
}
