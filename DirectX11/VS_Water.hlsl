struct VS_IN
{
	float3 pos : POSITION0;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
};


struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float4 WorldPos : POSITION0;
	float2 Tex : TEXCOORD0;
	float3 ViewDir : TEXCOORD1;

	float3 NormalW : TEXCOORD2;
	float4 TangentW : TEXCOORD3;
};

cbuffer WVP : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 proj;
}

cbuffer Wave:register(b1){
	float time;
	float waveSpeed;
	float waveAmplitude;
	float waveLength;
}

cbuffer EyePos : register(b2)
{
	float3 cameraPos;
	float padding;
}


VS_OUT main(VS_IN vin)
{
	VS_OUT vOut;

	float4 worldPos = mul(float4(vin.pos, 1.f), world);

	//===== Wave Param =====//
	float2 dir1 = normalize(float2(1, 0));
	float2 dir2 = normalize(float2(0.5f, 0.5f));

	//===== Gerstner Wave =====//
	float wave1 = waveAmplitude * sin(dot(dir1, worldPos.xz) / waveLength + time * waveSpeed);
	float wave2 = waveAmplitude * 0.5f * sin(dot(dir2, worldPos.xz) / (waveLength * 0.5f) + time * waveSpeed * 1.2f);
	worldPos.y += (wave1 + wave2);

	//====== Normal Calculation ======//
	// using height field to approximate normal
	float2 xz = worldPos.xz;

	float2 k1 = dir1 / waveLength;
	float phase1 = dot(k1, xz) + time * waveSpeed;
	float dWave1dx = waveAmplitude * cos(phase1) * k1.x;
	float dWave1dz = waveAmplitude * cos(phase1) * k1.y;

	float2 k2 = dir2 / (waveLength * 0.5f);
	float phase2 = dot(k2, xz) + time * waveSpeed * 1.2f;
	float dWave2dx = waveAmplitude * 0.5f * cos(phase2) * k2.x;
	float dWave2dz = waveAmplitude * 0.5f * cos(phase2) * k2.y;

	float dhdx = dWave1dx + dWave2dx;
	float dhdz = dWave1dz + dWave2dz;

	float3 normalW = normalize(float3(-dhdx, 1.0f, -dhdz));

	float3 tangentW = normalize(cross(float3(0, 0, 1), normalW));

	vOut.WorldPos = worldPos;
	vOut.PosH = mul(mul(worldPos, view), proj);
	vOut.Tex = vin.tex;
	vOut.ViewDir = cameraPos - worldPos.xyz;
	vOut.NormalW = normalW;
	vOut.TangentW = float4(tangentW, 0.0f);

	return vOut;
}