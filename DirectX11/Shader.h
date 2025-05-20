#include "D3DApp.h"
#include "Texture.h"
#include <string>
#include <map>
#include <vector>

// シェーダーの基本クラス
class Shader
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
protected:
	enum Kind
	{
		Vertex,
		Geometry,
		Hull,
		Domain,
		Pixel,
	};
protected:
	Shader(Kind kind);
public:
	virtual ~Shader();
public:
	// シェーダーファイル(*.cso)を読み込む処理
	HRESULT Load(const char* pFileName);
	// 文字列からシェーダをコンパイル
	HRESULT Compile(const char* pCode);


	// 定数の書き込み
	void WriteBuffer(UINT slot, void* pData);
	// テクスチャの設定
	void SetTexture(UINT slot, Texture* tex);
	// シェーダーを描画に使用
	virtual void Bind(void) = 0;

private:
	HRESULT Make(void* pData, UINT size);
protected:
	// シェーダーファイルを読み込んだ後、シェーダーの種類別に処理を行う
	virtual HRESULT MakeShader(void* pData, UINT size) = 0;

private:
	Kind m_kind;
protected:
	std::vector<ComPtr<ID3D11Buffer>> m_pBuffers;
	std::vector<ComPtr<ID3D11ShaderResourceView>> m_pTextures;
};

//----------
// 頂点シェーダ
class VertexShader : public Shader
{
public:
	VertexShader();
	~VertexShader();
	void Bind(void);
protected:
	HRESULT MakeShader(void* pData, UINT size);

private:
	ComPtr<ID3D11VertexShader> m_pVS;
	ComPtr<ID3D11InputLayout> m_pInputLayout;
};
//----------
// ピクセルシェーダ
class PixelShader : public Shader
{
public:
	PixelShader();
	~PixelShader();
	void Bind(void);
protected:
	HRESULT MakeShader(void* pData, UINT size);
private:
	ComPtr<ID3D11PixelShader> m_pPS;
};

//----------
// ハルシェーダー
class HullShader : public Shader
{
public:
	HullShader();
	~HullShader();
	void Bind();
	static void Unbind();
protected:
	HRESULT MakeShader(void* pData, UINT size);
private:
	ComPtr<ID3D11HullShader> m_pHS;
};

//----------
// ドメインシェーダー
class DomainShader : public Shader
{
public:
	DomainShader();
	~DomainShader();
	void Bind();
	static void Unbind();
protected:
	HRESULT MakeShader(void* pData, UINT size);
private:
	ComPtr<ID3D11DomainShader> m_pDS;
};

//----------
// ジオメトリシェーダー
class GeometryShader : public Shader
{
public:
	GeometryShader();
	~GeometryShader();
	void Bind();
	static void Unbind();
protected:
	HRESULT MakeShader(void* pData, UINT size);
private:
	ComPtr<ID3D11GeometryShader> m_pGS;
};



