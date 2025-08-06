#include "LightHelper.hlsli"
struct PS_IN
{
	float4 posH : SV_POSITION;
	float3 posL : POSITION;
};

//cbuffer CameraBuffer : register(b0)
//{
//	float3 tintColor;
//};

TextureCube skyboxTex : register(t0);
SamplerState mySampler : register(s0);

float4 main(PS_IN input) : SV_TARGET
{
	float4 baseColor = skyboxTex.Sample(mySampler, input.posL);

	//float brightness = 0.5f + 0.5f * cos(time * 2 * PI);
	//return baseColor * brightness;

	//float3 finalColor = baseColor.rgb * 0.8f + tintColor * 0.2f;
	//float3 finalColor = baseColor.rgb * tintColor;
	//return float4(saturate(finalColor), 1.0f);

	return baseColor;

}