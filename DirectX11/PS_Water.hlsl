#include "LightHelper.hlsli"

struct PS_IN
{
	float4 PosH : SV_POSITION;
	float4 WorldPos : POSITION0;
	float2 Tex : TEXCOORD0;
	float3 ViewDir : TEXCOORD1;

	float3 NormalW : TEXCOORD2;
	float4 TangentW : TEXCOORD3;
};

cbuffer Wave : register(b0)
{
	float3 deepColor;		// Deep water color (ãﬂÇ¢Ç«Ç±ÇÎ)
	float padding1;

	float3 shallowColor;	// Shallow water color(âìÇ¢Ç«Ç±ÇÎ)
	float padding2;

	float maxDistance; //Max distance for depth color blending
	float time;
	float padding3;
	float padding4;
}

cbuffer DayLight : register(b1)
{
	float4 lightAmbient;
	float4 lightDiffuse;
	float3 lightDir;
	float lightIntensity;
}


Texture2D normalMap : register(t0);
SamplerState samplerState : register(s0);

float4 main(PS_IN pin) : SV_Target
{
	float metallic = 0.0f;	// water has no metallic
	float smooth = 0.95f;	// water is very smooth
	float roughness = 1.0f - smooth;

	//======= Use uv animation for water normal =======//
	float2 uv1 = pin.Tex + float2(time * 0.01, 0);
	float2 uv2 = pin.Tex + float2(0, time * 0.01);
	float3 n1 = normalMap.Sample(samplerState, uv1).xyz;
	float3 n2 = normalMap.Sample(samplerState, uv2).xyz;

	//====== Normalize vtx normal&tangent ======//
	float3 normalW = normalize(pin.NormalW);
	float4 tangentW = normalize(pin.TangentW);
	float3 normalM = normalize(n1 + n2);

	//====== Calculate the normal in world space ======//
	float3 N = NormalSampleToWorldSpace(normalM, normalW, tangentW);

    // ãóó£ÇÃâàÇ¡ÇƒÅ@êFÇï‚ä‘
	float dist = length(pin.ViewDir);
	float t = saturate(dist / maxDistance);
	float3 baseColor = lerp(deepColor, shallowColor, t);

	// Fresnel effect
	float3 V = normalize(pin.ViewDir);
	float3 L = normalize(-lightDir); //Directional light direction

	float3 ambient = baseColor.rgb * lightAmbient.rgb * lerp(0.2, 0.05, metallic) * min(lightIntensity, 4.); // ambient contribution & avoid too much light intensity


	//Dir Light
	float3 directionalColor = DirectionLightPBR(lightIntensity, lightDiffuse.rgb, L, N, V, roughness, metallic, baseColor.xyz, 1.0f);
	float3 pbrColor = 0;
	pbrColor += directionalColor;

	float3 totalColor = pbrColor + ambient;
	return float4(totalColor, 1.0f);
}