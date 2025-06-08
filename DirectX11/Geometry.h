#pragma once
#include <memory>
#include "DirectXMath.h"
#include "MeshBuffer.h"

class VertexShader;
class PixelShader;
class Geometry
{
public:
	static void Init();
	static void Uninit();

	static void SetVertexShader(VertexShader* vs);
	static void SetPixelShader(PixelShader* ps);
	static void SetWorld(DirectX::XMFLOAT4X4 world);
	static void SetView(DirectX::XMFLOAT4X4 view);
	static void SetProjection(DirectX::XMFLOAT4X4 proj);
	static void SetColor(DirectX::XMFLOAT4 color);
	static void SetLightDirection(DirectX::XMFLOAT3 dir);
	static void EnableLight(bool enable);

	static void AddLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end);
	static void AddLine(
		DirectX::XMFLOAT3 startPos, DirectX::XMFLOAT4 startColor,
		DirectX::XMFLOAT3 endPos, DirectX::XMFLOAT4 endColor);
	static void DrawLines();
	static void DrawBox();
	static void DrawSphere();

private:
	static void MakeVertexShader();
	static void MakePixelShader();
	static void MakeLine();
	static void MakeBox();
	static void MakeSphere();

private:
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT3 normal;
	};
	struct Data
	{
		static const int LineNum = 10000 * 2; // 1直線につき、2頂点
		std::shared_ptr<VertexShader> defVS;
		std::shared_ptr<PixelShader> defPS;
		VertexShader* pVS;
		PixelShader* pPS;
		DirectX::XMFLOAT4X4 matrix[3];
		DirectX::XMFLOAT4 param[2];
		Vertex lineVtx[LineNum];
		UINT lineIndex;
		std::shared_ptr<MeshBuffer> lineMesh;
		std::shared_ptr<MeshBuffer> boxMesh;
		std::shared_ptr<MeshBuffer> sphereMesh;
	};
	static Data m_data;

};

