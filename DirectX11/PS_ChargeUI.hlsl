#include "LightHelper.hlsli"

/// @brief Rect Rendering Without Interaction
struct PS_IN
{
	float4 pos : SV_POSITION0;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
};

cbuffer Material : register(b0)
{
	float4 ambient;
	float4 diffuse;
	float isTexEnable;
	float progress; // [0,1]
	float pad2,pad3;
}


Texture2D myTex : register(t0);
SamplerState mySampler : register(s0);

float4 main(PS_IN pin) : SV_TARGET
{
		float2 dir = pin.tex - float2(0.5, 0.5);

	// from 12 clock
	float angle = atan2(dir.x, -dir.y);
	angle = (angle < 0.0f) ? angle + 6.2831853f : angle;

    // Convert progress to angle range
	float limit = progress * 6.2831853f;

	// discard pixel out of range;
	if (angle > limit)
		discard;

	float4 texColor = lerp(float4(1, 1, 1, 1), myTex.Sample(mySampler, pin.tex), isTexEnable);
	float4 color = texColor * ambient * diffuse; // ˆ½ŽÒ * lightingColor

	//Gamma correction
	clip(color.a - 0.1f);
	return color;

}