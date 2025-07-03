struct VS_IN
{
	float3 pos : POSITION0;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
};

struct VS_OUT
{
	float4 pos : SV_POSITION0;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;

};

cbuffer WVP : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 proj;
}

VS_OUT main(VS_IN vin)
{
	VS_OUT vOut;

	float4 worldPos = mul(float4(vin.pos, 1.0f), world);
	vOut.pos = mul(worldPos, view);
	vOut.pos = mul(vOut.pos, proj);

	vOut.normal = vin.normal;
	vOut.tex = vin.tex;
	return vOut;
}