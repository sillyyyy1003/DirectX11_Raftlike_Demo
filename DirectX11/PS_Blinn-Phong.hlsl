///@brief Blinn-Phong Lighting model with texture sampling

struct PS_IN
{
	float4 pos : SV_POSITION0;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
	float4 worldPos : POSITION0;
};

cbuffer lightBuffer : register(b0)
{
	float4 lightAmbient;
	float4 lightDiffuse;
	float4 lightPos;
	float4 lightDir;
}

cbuffer Material:register(b1)
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float isTexEnable;
	float pad1, pad2, pad3;
}

cbuffer CameraPos:register(b2){
	float4 cameraPos;
}

Texture2D myTex : register(t0);
SamplerState mySampler : register(s0);


float4 main(PS_IN pin) : SV_TARGET
{
	float4 color = float4(1, 1, 1, 1);
	color = lerp(diffuse, myTex.Sample(mySampler, pin.tex), isTexEnable);
	float3 N = normalize(pin.normal);
	float3 toEye = normalize(-cameraPos.xyz);

	//環境光計算
	float4 ambientColor = ambient * lightAmbient;
	float3 lightVec = normalize(lightPos.xyz);

    //Lambert Diffuse計算 ->half lambert
	float diffuseFactor = saturate(dot(lightVec, N)) * 0.5f + 0.5f;
	float4 diffuseColor = diffuseFactor * lightDiffuse * diffuse;

	// specular 計算
	float3 R = reflect(-lightVec, N); // 反射向量
	float3 V = normalize(cameraPos.xyz - pin.worldPos.xyz);
	float specFactor = pow(max(dot(R, V), 0.0f), specular.w);
	float4 spec = specFactor * specular * 0.1f;

	float4 litColor = color * (ambientColor + diffuseColor) + spec;

    //アルファ値をマテリアルに計算
	litColor.a = diffuse.a;
	return litColor;
}