#include "Geometry.h"
#include "Shader.h"
Geometry::Data Geometry::m_data;

void Geometry::Init()
{
	for (int i = 0; i < 3; ++i)
		DirectX::XMStoreFloat4x4(&m_data.matrix[i], DirectX::XMMatrixIdentity());
	m_data.param[0] = DirectX::XMFLOAT4(1, 1, 1, 1);
	m_data.param[1] = DirectX::XMFLOAT4(1, 1, 1, 1);

	MakeVertexShader();
	MakePixelShader();
	MakeLine();
	MakeBox();
	MakeSphere();
}
void Geometry::Uninit()
{
	m_data.sphereMesh.reset();
	m_data.boxMesh.reset();
	m_data.lineMesh.reset();
	m_data.defPS.reset();
	m_data.defVS.reset();
}

void Geometry::SetVertexShader(VertexShader* vs)
{
	m_data.pVS = vs;
}
void Geometry::SetPixelShader(PixelShader* ps)
{
	m_data.pPS = ps;
}
void Geometry::SetWorld(DirectX::XMFLOAT4X4 world)
{
	m_data.matrix[0] = world;
}
void Geometry::SetView(DirectX::XMFLOAT4X4 view)
{
	m_data.matrix[1] = view;
}
void Geometry::SetProjection(DirectX::XMFLOAT4X4 proj)
{
	m_data.matrix[2] = proj;
}
void Geometry::SetColor(DirectX::XMFLOAT4 color)
{
	m_data.param[0] = color;
}
void Geometry::SetLightDirection(DirectX::XMFLOAT3 dir)
{
	DirectX::XMVECTOR vDir = DirectX::XMLoadFloat3(&dir);
	DirectX::XMStoreFloat3(&dir, DirectX::XMVector3Normalize(vDir));
	m_data.param[1].x = dir.x;
	m_data.param[1].y = dir.y;
	m_data.param[1].z = dir.z;
}
void Geometry::EnableLight(bool enable)
{
	m_data.param[1].w = enable ? 0.0f : 1.0f;
}

void Geometry::AddLine(const DirectX::XMFLOAT3 start, const DirectX::XMFLOAT3 end)
{
	if (m_data.lineIndex < Data::LineNum)
	{
		m_data.lineVtx[m_data.lineIndex].pos = start;
		m_data.lineVtx[m_data.lineIndex].color = m_data.param[0];
		++m_data.lineIndex;
		m_data.lineVtx[m_data.lineIndex].pos = end;
		m_data.lineVtx[m_data.lineIndex].color = m_data.param[0];
		++m_data.lineIndex;
	}
}
void Geometry::AddLine(
	DirectX::XMFLOAT3 startPos, DirectX::XMFLOAT4 startColor,
	DirectX::XMFLOAT3 endPos, DirectX::XMFLOAT4 endColor)
{
	if (m_data.lineIndex < Data::LineNum)
	{
		m_data.lineVtx[m_data.lineIndex].pos = startPos;
		m_data.lineVtx[m_data.lineIndex].color = startColor;
		++m_data.lineIndex;
		m_data.lineVtx[m_data.lineIndex].pos = endPos;
		m_data.lineVtx[m_data.lineIndex].color = endColor;
		++m_data.lineIndex;
	}
}
void Geometry::DrawLines()
{
	DirectX::XMFLOAT4X4 worldBackup = m_data.matrix[0];
	DirectX::XMFLOAT4 colorBackup = m_data.param[0];
	float lightBackup = m_data.param[1].w;

	DirectX::XMStoreFloat4x4(&m_data.matrix[0], DirectX::XMMatrixIdentity());
	m_data.param[0] = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_data.param[1].w = 1.0f;

	m_data.pVS->WriteBuffer(0, m_data.matrix);
	m_data.pVS->WriteBuffer(1, m_data.param);
	m_data.pVS->Bind();
	m_data.pPS->Bind();
	m_data.lineMesh->Write(m_data.lineVtx);
	m_data.lineMesh->Draw(m_data.lineIndex);

	m_data.matrix[0] = worldBackup;
	m_data.param[0] = colorBackup;
	m_data.param[1].w = lightBackup;

	m_data.lineIndex = 0;
}
void Geometry::DrawBox()
{
	m_data.pVS->WriteBuffer(0, m_data.matrix);
	m_data.pVS->WriteBuffer(1, m_data.param);
	m_data.pVS->Bind();
	m_data.pPS->Bind();
	m_data.boxMesh->Draw();
}
void Geometry::DrawSphere()
{
	m_data.pVS->WriteBuffer(0, m_data.matrix);
	m_data.pVS->WriteBuffer(1, m_data.param);
	m_data.pVS->Bind();
	m_data.pPS->Bind();
	m_data.sphereMesh->Draw();
}

void Geometry::MakeVertexShader()
{
	const char* VSCode = R"EOT(
struct VS_IN {
	float3 pos : POSITION0;
	float4 color : COLOR0;
	float3 normal : NORMAL0;
};
struct VS_OUT {
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
};
cbuffer Matrix : register(b0) {
	float4x4 world;
	float4x4 view;
	float4x4 proj;
};
cbuffer Param : register(b1) {
	float4 color;
	float4 light;
};
VS_OUT main(VS_IN vin) {
	VS_OUT vout;
	vout.pos = float4(vin.pos, 1.0f);
	vout.pos = mul(vout.pos, world);
	vout.pos = mul(vout.pos, view);
	vout.pos = mul(vout.pos, proj);
	
	float3 N = normalize(mul(vin.normal, (float3x3)world));
	float3 L = normalize(light.xyz);
	float diffuse = dot(N, L) * 0.5f + 0.5f;
	diffuse = saturate(diffuse + light.w);
	vout.color = vin.color * color * diffuse;

	return vout;
})EOT";
	m_data.defVS = std::make_shared<VertexShader>();
	HRESULT hr = m_data.defVS->Compile(VSCode);
	m_data.pVS = m_data.defVS.get();
}

void Geometry::MakePixelShader()
{
	const char* PSCode = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
};
float4 main(PS_IN pin) : SV_TARGET0 {
	return pin.color;
})EOT";
	m_data.defPS = std::make_shared<PixelShader>();
	HRESULT hr = m_data.defPS->Compile(PSCode);
	m_data.pPS = m_data.defPS.get();
}

void Geometry::MakeLine()
{
	MeshBuffer::MeshData desc = {};
	for (int i = 0; i < Data::LineNum; ++i)
	{
		m_data.lineVtx[i].pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_data.lineVtx[i].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		m_data.lineVtx[i].normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	}
	desc.pVertex = m_data.lineVtx;
	desc.vertexSize = sizeof(Vertex);
	desc.vertexCount = Data::LineNum;
	desc.isWrite = true;
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	m_data.lineIndex = 0;
	m_data.lineMesh = std::make_shared<MeshBuffer>(desc);
}
void Geometry::MakeBox()
{
	const float d = 0.5f;
	DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT3 normal[] = {
		DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
		DirectX::XMFLOAT3(0.0f,-1.0f, 0.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f,-1.0f),
	};
	Vertex vtx[] = {
		// +X
		{DirectX::XMFLOAT3(d, d,-d), color, normal[0]},
		{DirectX::XMFLOAT3(d, d, d), color, normal[0]},
		{DirectX::XMFLOAT3(d,-d,-d), color, normal[0]},
		{DirectX::XMFLOAT3(d,-d, d), color, normal[0]},
		// -X
		{DirectX::XMFLOAT3(-d, d, d), color, normal[1]},
		{DirectX::XMFLOAT3(-d, d,-d), color, normal[1]},
		{DirectX::XMFLOAT3(-d,-d, d), color, normal[1]},
		{DirectX::XMFLOAT3(-d,-d,-d), color, normal[1]},
		// +Y
		{DirectX::XMFLOAT3(-d, d, d), color, normal[2]},
		{DirectX::XMFLOAT3(d, d, d), color, normal[2]},
		{DirectX::XMFLOAT3(-d, d,-d), color, normal[2]},
		{DirectX::XMFLOAT3(d, d,-d), color, normal[2]},
		// -Y
		{DirectX::XMFLOAT3(-d,-d,-d), color, normal[3]},
		{DirectX::XMFLOAT3(d,-d,-d), color, normal[3]},
		{DirectX::XMFLOAT3(-d,-d, d), color, normal[3]},
		{DirectX::XMFLOAT3(d,-d, d), color, normal[3]},
		// +Z
		{DirectX::XMFLOAT3(-d, d, d), color, normal[4]},
		{DirectX::XMFLOAT3(d, d, d), color, normal[4]},
		{DirectX::XMFLOAT3(-d,-d, d), color, normal[4]},
		{DirectX::XMFLOAT3(d,-d, d), color, normal[4]},
		// -Z
		{DirectX::XMFLOAT3(-d,-d,-d), color, normal[5]},
		{DirectX::XMFLOAT3(d,-d,-d), color, normal[5]},
		{DirectX::XMFLOAT3(-d, d,-d), color, normal[5]},
		{DirectX::XMFLOAT3(d, d,-d), color, normal[5]},
	};
	WORD idx[] = {
		 2, 0, 1,  1, 3, 2,
		 6, 4, 5,  5, 7, 6,
		10, 8, 9,  9,11,10,
		14,12,13, 13,15,14,
		18,16,17, 17,19,18,
		22,20,21, 21,23,22,
	};
	MeshBuffer::MeshData desc = {};
	desc.pVertex = vtx;
	desc.vertexSize = sizeof(Vertex);
	desc.vertexCount = _countof(vtx);
	desc.pIndex = idx;
	desc.indexSize = sizeof(idx[0]);
	desc.indexCount = _countof(idx);
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_data.boxMesh = std::make_shared<MeshBuffer>(desc);
}
void Geometry::MakeSphere()
{
	std::vector<Vertex> vtx;
	const int detail = 32;
	for (int j = 0; j < detail / 2 + 1; ++j)
	{
		float radY = DirectX::XM_2PI / detail * j;
		for (int i = 0; i < detail; ++i)
		{
			float radXZ = DirectX::XM_2PI / detail * i;
			float x = sinf(radY) * sinf(radXZ);
			float y = cosf(radY);
			float z = sinf(radY) * cosf(radXZ);
			vtx.push_back({
				DirectX::XMFLOAT3(x * 0.5f, y * 0.5f, z * 0.5f),
				DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				DirectX::XMFLOAT3(x, y, z)
				});
		}
	}
	std::vector<DWORD> idx;
	for (int j = 0; j < detail / 2; ++j)
	{
		for (int i = 0; i < detail; ++i)
		{
			idx.push_back((j + 1) * detail + i);
			idx.push_back(j * detail + i);
			idx.push_back(j * detail + ((i + 1) % detail));
			idx.push_back(idx.back());
			idx.push_back((j + 1) * detail + ((i + 1) % detail));
			idx.push_back((j + 1) * detail + i);
		}
	}
	MeshBuffer::MeshData desc = {};
	desc.pVertex = vtx.data();
	desc.vertexSize = sizeof(Vertex);
	desc.vertexCount = static_cast<UINT>(vtx.size());
	desc.pIndex = idx.data();
	desc.indexSize = sizeof(idx[0]);
	desc.indexCount = static_cast<UINT>(idx.size());
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_data.sphereMesh = std::make_shared<MeshBuffer>(desc);
}