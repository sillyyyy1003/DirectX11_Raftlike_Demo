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
	float pad1, pad2, pad3;
}

Texture2D myTex : register(t0);
SamplerState mySampler : register(s0);

float4 main(PS_IN pin) : SV_TARGET
{
	float4 color = float4(1, 1, 1, 1);
	color = lerp(diffuse, myTex.Sample(mySampler, pin.tex),isTexEnable);

	color *= diffuse;
	color *= ambient;
	clip(color.a - 0.1f);

	return color;

}