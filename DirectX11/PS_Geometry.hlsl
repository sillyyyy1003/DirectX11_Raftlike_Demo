//Use Lambert Model

struct PS_IN
{
	float4 pos : SV_POSITION0;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
	float4 worldPos : POSITION0;
};

float4 main(PS_IN pin) : SV_TARGET
{
	float4 color = float4(1, 1, 1, 1);
	return color;
}