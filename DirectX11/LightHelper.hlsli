//Direction Light

static const float F0_NON_METAL = 0.04;
static const float PI = 3.14159265358979323846f;
struct DirectionLight
{
	float4 ambient;
	float4 diffuse;
	float3 direction;
	float isEnable;
  
};

//Point Light
struct PointLight
{
	float4 ambient;
	float4 diffuse;

	float3 position;
	float range;

	float3 att;
	float isEnable;
};

//Spotlight
struct SpotLight
{
	float4 ambient;
	float4 diffuse;

	float3 position;
	float range;

	float3 direction;
	float Spot;

	float3 att;
	float isEnable;
};

struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 specular; // w = SpecPower
	float4 emission;
	float isTexEnable;
	float Dummy[3];
};

#define MAX_NUM_POINT_LIGHT (2)

//todo:後で光の貫通問題を解決する
/// @brief 点光源の計算
/// @param light 点光源の情報
/// @param mat マテリアル
/// @param normal 法線
/// @param worldPos 位置
/// @param eyePos カメラ位置
/// @param ambient 出力するAmbient Color
/// @param diffuse 出力するDiffuse Color
/// @param specular 出力するHighlight Color
void CalculatePointLight(PointLight light, Material mat, float3 normal, float4 worldPos, float4 eyePos,
out float4 ambient,
out float4 diffuse,
out float4 specular)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 1.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 1.0f);

    //ライトあるかどうか？
	if (light.isEnable == 0)
		return;

	float3 lightVec = light.position.xyz - worldPos.xyz;
	float distance = length(lightVec);

    //ライト範囲判定
	if (distance > light.range)
		return;

    //Calculate ambient
	ambient = mat.ambient * light.ambient;

    //Calculate diffuse factor(half lambert model)
	float diffuseFactor = saturate(dot(normalize(lightVec), normal));
	diffuse = diffuseFactor * mat.diffuse * light.diffuse;

    ///Calculate specualr(bling-phong)
	float3 toEye = normalize(-eyePos.xyz);
	float3 v = normalize(eyePos.xyz - worldPos.xyz);
	float specFactor = pow(max(dot(v, toEye), 0.0f), mat.specular.w);
	specular = specFactor * mat.specular;
	//減衰計算
	//float att = 1.0f / dot(light.att, float3(1.0f, distance, distance * distance));
	float att = saturate(1.0f - distance / light.range);
	att = pow(att, 2.0f);

	ambient *= att;
	diffuse *= att;
	specular *= att;
}


/// @brief 点ライトの計算
/// @param mat マテリアル
/// @param L LightVector
/// @param pos 位置
/// @param normal 法線ベクトル
/// @param toEye -CameraPos
/// @param ambient 環境光
/// @param diffuse ライト色
/// @param spec 鏡面
void ComputePointLight(Material mat, PointLight L, float3 pos, float3 normal, float3 toEye,
                        out float4 ambient,
                        out float4 diffuse,
                        out float4 spec)
{
   
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//ライトチェック
	if (L.isEnable == 0.0f)
	{
		return;
	}
    
	//環境光計算
	ambient = mat.ambient * L.ambient;


	float3 lightVec = L.position - pos;

    //ライト範囲計算・比較
	float d = length(lightVec);
	if (d > L.range)
		return;

    //正規化
	lightVec = normalize(lightVec);

    //反射角度計算
	float diffuseFactor = saturate(dot(lightVec, normal));


	float3 v = reflect(-lightVec, normal);
	float specFactor = pow(max(dot(v, toEye), 0.0f), mat.specular.w);

	diffuse = diffuseFactor * mat.diffuse * L.diffuse;
	spec = specFactor * mat.specular;


    //減衰計算
	float att = 1.0f / dot(L.att, float3(1.0f, d, d * d));

	diffuse *= att;
	spec *= att;
}

/// @brief スポットライトの計算
/// @param mat マテリアル
/// @param L LightVector
/// @param pos ピクセル位置
/// @param normal 法線ベクトル
/// @param toEye -CameraPos
/// @param ambient 環境光
/// @param diffuse ライト色
/// @param spec 鏡面
void ComputeSpotLight(Material mat, SpotLight L, float3 pos, float3 normal, float3 toEye,
                       out float4 ambient,
                       out float4 diffuse,
                       out float4 spec)
{

	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//ライトチェック
	if (L.isEnable == 0.0f)
	{
		return;
	}

	//環境光計算
	ambient = mat.ambient * L.ambient;

	//ライト範囲計算・比較
	float3 lightVec = L.position - pos;
	float d = length(lightVec);
	if (d > L.range)
		return;

	//正規化
	lightVec = normalize(lightVec);

    //反射角度
	float diffuseFactor = saturate(dot(lightVec, normal));

	float3 v = reflect(-lightVec, normal);
	float specFactor = pow(max(dot(v, toEye), 0.0f), mat.specular.w);

	diffuse = diffuseFactor * mat.diffuse * L.diffuse;
	spec = specFactor * mat.specular;

    //減衰計算
	float spot = pow(max(dot(-lightVec, L.direction), 0.0f), L.Spot);
	float att = spot / dot(L.att, float3(1.0f, d, d * d));

	ambient *= spot;
	diffuse *= att;
	spec *= att;
}

float3 NormalSampleToWorldSpace(float3 normalMapSample,
    float3 unitNormalW,
    float4 tangentW)
{
	float3 normalT = 2.0f * normalMapSample - 1.0f;

	float3 N = unitNormalW;
	float3 T = normalize(tangentW.xyz - dot(tangentW.xyz, N) * N);
	float3 B = cross(N, T);

	float3x3 TBN = float3x3(T, B, N);

	float3 bumpedNormalW = mul(normalT, TBN);

	return bumpedNormalW;
}

float4 Gamma(float4 color)
{
	return pow(color, 1.f / 2.2f);
}


float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = saturate(dot(N, H));
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
	denom = PI * denom * denom;

	return nom / max(denom, 0.0000001);
}

// Schlick
float3 SchlickFresnel(float3 F0, float3 H, float3 V)
{
	float cosIncidentAngle = saturate(dot(H, V));

	return F0 + (1.0f - F0) * pow(1 - cosIncidentAngle, 5);
}

// 
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
   
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

// Cook-Torrance 
float3 CookTorranceSpecular(float3 N, float3 L, float3 V, float roughness, float3 F0, out float3 Ks)
{
	float3 h = normalize(V + L);

	float D = DistributionGGX(N, h, roughness);
	float3 F = SchlickFresnel(F0, h, V);
	Ks = F;
	float G = GeometrySmith(N, V, L, roughness);
	float NdotV = max(dot(N, V), 0.0f);
	float NdotL = max(dot(N, L), 0.0f);
	return (D * F * G) / (4 * max(NdotV * NdotL, 0.01f));
}

// Lambert diffuse
float3 LambertDiffuse(float3 Ks, float3 albedo, float metallic)
{
	float3 Kd = (float3(1.0f, 1.0f, 1.0f) - Ks) * (1 - metallic);
	return (Kd * albedo / PI);
}

// Environment Light PBR
float3 EnvironmentPBR(float lightIntensity, float3 lightColor, float3 toLight, float3 normal, float3 toEyeW_Unit, float roughness, float metallic, float3 albedo, float shadowAmount)
{
	float3 f0 = lerp(F0_NON_METAL.rrr, albedo.rgb, metallic);
	float3 kS = float3(0.f, 0.f, 0.f);
	float3 specularBRDF = CookTorranceSpecular(normal, toLight, toEyeW_Unit, roughness, f0, kS);
	float3 diffuseBRDF = LambertDiffuse(kS, albedo, metallic);
	return (diffuseBRDF + specularBRDF) * lightIntensity * lightColor.rgb * shadowAmount;
}

// Directional Light PBR
float3 DirectionLightPBR(float lightIntensity, float3 lightColor, float3 toLight, float3 normal, float3 toEyeW_Unit, float roughness, float metallic, float3 albedo, float shadowAmount)
{
	float3 f0 = lerp(F0_NON_METAL.rrr, albedo.rgb, metallic);
	float3 kS = float3(0.f, 0.f, 0.f);
	float3 specularBRDF = CookTorranceSpecular(normal, toLight, toEyeW_Unit, roughness, f0, kS);
	float3 diffuseBRDF = LambertDiffuse(kS, albedo, metallic);

	float NdotL = max(dot(normal, toLight), 0.0);
	return (diffuseBRDF + specularBRDF) * lightIntensity * lightColor.rgb * shadowAmount * NdotL;
}

float3 PointLightPBR(PointLight pointLight, float3 pos, float3 normal, float3 toEyeW_Unit)
{
	float3 color = float3(0, 0, 0);
	if (pointLight.isEnable == 0.f)
	{
		return color;
	}
	else
	{
		//calculate vertex to pointLight vector
		float3 toLightVec = pointLight.position - pos;
		float3 V = normalize(toLightVec);
		float3 toLightLen = length(toLightVec);
		//calculate attenuation
		float3 N = normalize(normal);
		float dotNV = saturate(dot(N, V));
		float3 attenuation = saturate(1.0f - toLightLen / pointLight.range);
		attenuation *= pointLight.att.x;
		attenuation = pow(attenuation, 2.f);

		color = pointLight.diffuse.rgb * dotNV * attenuation;
		return color;
	}
	
}