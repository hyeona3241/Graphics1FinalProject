////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"

#include <fstream>
#include <vector>
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
	    XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct InstanceType
	{
		XMFLOAT3 position;  // 12바이트
		float pad1;         // 4바이트 (정렬용 패딩)
		XMFLOAT3 scale;     // 12바이트
		float pad2;         // 4바이트 (정렬용 패딩)
	};


	struct FaceType
	{
		int vIndex1, vIndex2, vIndex3;
		int tIndex1, tIndex2, tIndex3;
		int nIndex1, nIndex2, nIndex3;
	};

public:
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, const WCHAR*, const WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

	bool LoadModel(const WCHAR*);
	void ReleaseModel();

	bool ReadFileCounts(const WCHAR*);
	bool LoadDataStructures(const WCHAR*, int, int, int, int);

	bool InitializeBuffers(ID3D11Device*);
	bool LoadTexture(ID3D11Device*, const WCHAR*);
private:

	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	void ReleaseTexture();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_textureCount, m_normalCount, m_faceCount;
	TextureClass* m_Texture;

public:
	int m_vertexCount, m_indexCount;
	ModelType* m_model;

public:
	void SetupInstancing(ID3D11Device* device, int instanceCount, XMFLOAT3 startPosition);
	int GetInstanceCount() const;

private:
	ID3D11Buffer* m_instanceBuffer = nullptr;
	int m_instanceCount = 1; // 기본값 1 (복제 안됨)
};

#endif