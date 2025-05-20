struct VS_IN
{
	float3 pos : POSITION;
	float4 color : COLOR;
};

struct VS_OUT
{
	float4 posH : SV_POSITION;
	float4 color : COLOR;
};