////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "textureshaderclass.h"
#include <vector>
#include <string>
#include "timerclass.h"
#include "textclass.h"
#include "TitleScreen.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


struct VertexType
{
	XMFLOAT3 position;
	XMFLOAT2 texcoord;
};

////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
	bool Frame(int, int);

	void SetAnisotropicFilter(bool enable);

private:
	bool Render(float);

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	TextureShaderClass* m_TextureShader;

	std::vector<ModelClass*> m_Models;
	ModelClass* m_ModelGround;

	ModelClass* m_InstanceModel = nullptr;

private:

	ModelClass* m_SkyboxModel;

	// 스카이박스용
	ID3D11Buffer* m_skyboxVertexBuffer = nullptr;
	ID3D11Buffer* m_skyboxIndexBuffer = nullptr;
	int m_skyboxIndexCount = 0;
	ID3D11ShaderResourceView* m_skyboxTexture = nullptr;

	bool InitializeSkybox(ID3D11Device* device);
	void RenderSkybox(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

private:
	ID3D11Buffer* m_billboardVertexBuffer = nullptr;
	ID3D11Buffer* m_billboardIndexBuffer = nullptr;
	int m_billboardIndexCount = 0;

	TimerClass* m_Timer = nullptr;

	ID3D11ShaderResourceView* m_billboardTexture1 = nullptr;
	ID3D11ShaderResourceView* m_billboardTexture2 = nullptr;

	bool InitializeBillboards(ID3D11Device* device);
	void RenderBillboards(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

	void HandleInput();

	TextClass* m_Text;

	int m_screenWidth;
	int m_screenHeight;

	TitleScreen* m_TitleScreen;
	bool m_ShowTitle;

	int m_totalPolygonCount;

public:
	void SetTimer(TimerClass* timer);
};

#endif