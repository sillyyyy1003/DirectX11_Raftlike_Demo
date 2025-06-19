//Direction Light

static const float F0_NON_METAL = 0.04;
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

//todo:��Ō��̊ђʖ�����������
/// @brief �_�����̌v�Z
/// @param light �_�����̏��
/// @param mat �}�e���A��
/// @param normal �@��
/// @param worldPos �ʒu
/// @param eyePos �J�����ʒu
/// @param ambient �o�͂���Ambient Color
/// @param diffuse �o�͂���Diffuse Color
/// @param specular �o�͂���Highlight Color
void CalculatePointLight(PointLight light, Material mat, float3 normal, float4 worldPos, float4 eyePos,
out float4 ambient,
out float4 diffuse,
out float4 specular)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 1.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 1.0f);

    //���C�g���邩�ǂ����H
	if (light.isEnable == 0)
		return;

	float3 lightVec = light.position.xyz - worldPos.xyz;
	float distance = length(lightVec);

    //���C�g�͈͔���
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
	//�����v�Z
	//float att = 1.0f / dot(light.att, float3(1.0f, distance, distance * distance));
	float att = saturate(1.0f - distance / light.range);
	att = pow(att, 2.0f);

	ambient *= att;
	diffuse *= att;
	specular *= att;
}


/// @brief �_���C�g�̌v�Z
/// @param mat �}�e���A��
/// @param L LightVector
/// @param pos �ʒu
/// @param normal �@���x�N�g��
/// @param toEye -CameraPos
/// @param ambient ����
/// @param diffuse ���C�g�F
/// @param spec ����
void ComputePointLight(Material mat, PointLight L, float3 pos, float3 normal, float3 toEye,
                        out float4 ambient,
                        out float4 diffuse,
                        out float4 spec)
{
   
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//���C�g�`�F�b�N
	if (L.isEnable == 0.0f)
	{
		return;
	}
    
	//�����v�Z
	ambient = mat.ambient * L.ambient;


	float3 lightVec = L.position - pos;

    //���C�g�͈͌v�Z�E��r
	float d = length(lightVec);
	if (d > L.range)
		return;

    //���K��
	lightVec = normalize(lightVec);

    //���ˊp�x�v�Z
	float diffuseFactor = saturate(dot(lightVec, normal));


	float3 v = reflect(-lightVec, normal);
	float specFactor = pow(max(dot(v, toEye), 0.0f), mat.specular.w);

	diffuse = diffuseFactor * mat.diffuse * L.diffuse;
	spec = specFactor * mat.specular;


    //�����v�Z
	float att = 1.0f / dot(L.att, float3(1.0f, d, d * d));

	diffuse *= att;
	spec *= att;
}

/// @brief �X�|�b�g���C�g�̌v�Z
/// @param mat �}�e���A��
/// @param L LightVector
/// @param pos �s�N�Z���ʒu
/// @param normal �@���x�N�g��
/// @param toEye -CameraPos
/// @param ambient ����
/// @param diffuse ���C�g�F
/// @param spec ����
void ComputeSpotLight(Material mat, SpotLight L, float3 pos, float3 normal, float3 toEye,
                       out float4 ambient,
                       out float4 diffuse,
                       out float4 spec)
{

	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//���C�g�`�F�b�N
	if (L.isEnable == 0.0f)
	{
		return;
	}

	//�����v�Z
	ambient = mat.ambient * L.ambient;

	//���C�g�͈͌v�Z�E��r
	float3 lightVec = L.position - pos;
	float d = length(lightVec);
	if (d > L.range)
		return;

	//���K��
	lightVec = normalize(lightVec);

    //���ˊp�x
	float diffuseFactor = saturate(dot(lightVec, normal));

	float3 v = reflect(-lightVec, normal);
	float specFactor = pow(max(dot(v, toEye), 0.0f), mat.specular.w);

	diffuse = diffuseFactor * mat.diffuse * L.diffuse;
	spec = specFactor * mat.specular;

    //�����v�Z
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
