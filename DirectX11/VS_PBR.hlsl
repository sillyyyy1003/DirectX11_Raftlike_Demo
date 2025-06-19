struct VS_IN
{
	float3 pos : POSITION0;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
	float3 tangent : TANGENT0;
};

struct VS_OUT
{
	float4 pos : SV_POSITION0;
	float3 normal : NORMAL0;
	float2 tex : TEXCOORD0;
	float4 tangentW : TANGENT;
	float4 worldPos : POSITION0;
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
	vOut.pos = float4(vin.pos, 1.0f);
	vOut.pos = mul(vOut.pos, world);
	float4 worldPos = vOut.pos; // 計算途中でワールド座標の情報を変数にコピー
	vOut.worldPos = worldPos;
	vOut.pos = mul(vOut.pos, view);
	vOut.pos = mul(vOut.pos, proj);
	vOut.tex = vin.tex;
	vOut.normal = mul(vin.normal, (float3x3) world);

	vOut.tangentW = float4(mul(vin.tangent.xyz, (float3x3) world), 1.0);

	return vOut;
}