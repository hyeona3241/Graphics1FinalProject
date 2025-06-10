////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_TextureShader = 0;

}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	//m_Camera->SetPosition(0.0f, 0.0f, -5.0f);	// for cube model
//	m_Camera->SetPosition(0.0f, 0.5f, -3.0f);	// for chair model

	m_Camera->SetPosition(0.0f, 3.0f, -8.0f); 
	m_Camera->SetRotation(15.0f, 0.0f, 0.0f);
	
	InitializeSkybox(m_D3D->GetDevice());
	InitializeBillboards(m_D3D->GetDevice());

	std::vector<std::pair<std::wstring, std::wstring>> modelFiles = {
	//관
	{L"./data/ornate.obj",      L"./data/ornate.dds"},
	//관2
	{L"./data/ornate2.obj", L"./data/ornate2.dds"},
	//동상
	{L"./data/statue.obj",       L"./data/statue.dds"},
	//벽
	{L"./data/wall.obj",      L"./data/wall.dds"},
	//아누비스
	{L"./data/Anubis2.obj",      L"./data/Anubis2.dds"},
	//아누비스 동상
	{L"./data/Anubis.obj",      L"./data/Anubis.dds"},
	//제단
	{L"./data/ritual.obj", L"./data/ritual.dds"},
	//투탕카멘
	{L"./data/tstatue.obj",       L"./data/tstatue.dds"},
	//항아리2
	{L"./data/jar2.obj",      L"./data/jar2.dds"},
	//항아리4
	{L"./data/jar4.obj",      L"./data/jar4.dds"}
	};

	for (const auto& pair : modelFiles)
	{
		ModelClass* model = new ModelClass;
		if (!model)
			return false;

		if (!model->Initialize(m_D3D->GetDevice(),
			pair.first.c_str(),
			pair.second.c_str()))
		{
			MessageBox(hwnd, L"Could not initialize a model.", L"Error", MB_OK);
			return false;
		}

		m_Models.push_back(model);
	}


	XMFLOAT3 startPosition = XMFLOAT3(-7.0f, 2.0f, 11.0f);
	m_InstanceModel = m_Models[5];
	m_InstanceModel->SetupInstancing(m_D3D->GetDevice(), 10, startPosition); // 10개의 복제본, 시작 위치

	m_ModelGround = new ModelClass;
	if (!m_ModelGround)
		return false;

	ModelClass::ModelType* groundVerts = new ModelClass::ModelType[6]{
		{ -20, 0,  20,   0,0,   0,1,0 },
		{  20, 0,  20,   5,0,   0,1,0 },
		{  20, 0, -20,   5,5,   0,1,0 },
		
		{ -20, 0,  20,   0,0,   0,1,0 },
		{  20, 0, -20,   5,5,   0,1,0 },
		{ -20, 0, -20,   0,5,   0,1,0 },
	};

	m_ModelGround->m_vertexCount = 6; 
	m_ModelGround->m_indexCount = 6;
	m_ModelGround->m_model = groundVerts;

	if (!m_ModelGround->InitializeBuffers(m_D3D->GetDevice()))
	{
		MessageBox(hwnd, L"Ground buffer creation failed.", L"Error", MB_OK);
		return false;
	}

	if (!m_ModelGround->LoadTexture(m_D3D->GetDevice(), L"./data/tile.dds"))
	{
		MessageBox(hwnd, L"Ground texture loading failed.", L"Error", MB_OK);
		return false;
	}



	// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;
	if(!m_TextureShader)
	{
		return false;
	}

	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	//// 아누비스 모델만 인스턴싱 설정
	//m_Models[5]->SetupInstancing(m_D3D->GetDevice(), 10); // 예: 10개 복제

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the texture shader object.
	if(m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	for (auto& model : m_Models)
	{
		if (model)
		{
			model->Shutdown();
			delete model;
			model = nullptr;
		}
	}
	m_Models.clear(); // 벡터 비우기

	if (m_ModelGround)
	{
		m_ModelGround->Shutdown();
		delete m_ModelGround;
		m_ModelGround = 0;
	}


	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


bool GraphicsClass::Frame()
{
	bool result;

	static float rotation = 0.0f;

	HandleInput();
	m_Camera->UpdateCamera();


	// Update the rotation variable each frame.
	rotation += (float)XM_PI * 0.01f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Render the graphics scene.
	result = Render(rotation);
	if(!result)
	{
		return false;
	}

	return true;
}

void GraphicsClass::SetAnisotropicFilter(bool enable)
{
	m_TextureShader->SetAnisotropicFilter(enable);
}


bool GraphicsClass::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get view and projection matrices.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	RenderSkybox(m_D3D->GetDeviceContext(), viewMatrix, projectionMatrix);



	ID3D11DeviceContext* context = m_D3D->GetDeviceContext();

	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	ID3D11DepthStencilState* dsState = nullptr;
	HRESULT hr = m_D3D->GetDevice()->CreateDepthStencilState(&dsDesc, &dsState);
	if (SUCCEEDED(hr))
	{
		context->OMSetDepthStencilState(dsState, 1);
		dsState->Release();
	}



	//XMMATRIX viewMatrix, projectionMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	RenderBillboards(m_D3D->GetDeviceContext(), viewMatrix, projectionMatrix);

	//여기서 모델링
	for (size_t i = 0; i < m_Models.size(); ++i)
	{
		XMMATRIX worldMatrix;
		m_D3D->GetWorldMatrix(worldMatrix);

		if (i == 0) // 0번 모델만 특별한 배치
		{
			worldMatrix *= XMMatrixScaling(2.0f, 2.0f, 2.0f);    // 크기: 1배
			//worldMatrix *= XMMatrixRotationY(rotation);          // Y축 회전
			worldMatrix *= XMMatrixRotationY(XMConvertToRadians(90.0f)); // Y축 90도 회전
			worldMatrix *= XMMatrixTranslation(-3.5f, 1.0f, 4.0f); // X: -10으로 이동
		}
		else if (i == 1) // 1번 모델만 다른 배치
		{
			worldMatrix *= XMMatrixScaling(2.0f, 2.0f, 2.0f);    // 크기: 3배
			//worldMatrix *= XMMatrixRotationY(rotation * 0.5f);   // Y축 회전 (속도 절반)
			worldMatrix *= XMMatrixRotationY(XMConvertToRadians(90.0f)); // Y축 90도 회전
			worldMatrix *= XMMatrixTranslation(3.5f, 1.0f, 4.0f);  // X: 5, Z: 2
		}
		else if (i == 2)
		{
			worldMatrix *= XMMatrixScaling(1.5f, 1.5f, 1.5f);    // 크기: 3배
			//worldMatrix *= XMMatrixRotationY(rotation * 0.5f);   // Y축 회전 (속도 절반)
			//worldMatrix *= XMMatrixRotationY(XMConvertToRadians(90.0f)); // Y축 90도 회전
			worldMatrix *= XMMatrixTranslation(2.0f, 1.5f, 0.5f);  // X: 5, Z: 2
		}
		else if (i == 3)
		{
			worldMatrix *= XMMatrixScaling(5.0f, 5.0f, 5.0f);    // 크기: 3배
			//worldMatrix *= XMMatrixRotationY(rotation * 0.5f);   // Y축 회전 (속도 절반)
			worldMatrix *= XMMatrixRotationY(XMConvertToRadians(90.0f)); // Y축 90도 회전
			worldMatrix *= XMMatrixTranslation(0.0f, 2.0f, 15.0f);  // X: 5, Z: 2
		}
		//아누비스
		else if (i == 4)
		{
			worldMatrix *= XMMatrixScaling(1.3f, 1.3f, 1.3f);    // 크기: 3배
			//worldMatrix *= XMMatrixRotationY(rotation * 0.5f);   // Y축 회전 (속도 절반)
			//worldMatrix *= XMMatrixRotationY(XMConvertToRadians(90.0f)); // Y축 90도 회전
			worldMatrix *= XMMatrixTranslation(-2.0f, 1.3f, 0.5f);  // X: 5, Z: 2
		}
		else if (i == 6)
		{
			worldMatrix *= XMMatrixScaling(2.0f, 2.0f, 2.0f);    // 크기: 3배
			//worldMatrix *= XMMatrixRotationY(rotation * 0.5f);   // Y축 회전 (속도 절반)
			//worldMatrix *= XMMatrixRotationY(XMConvertToRadians(90.0f)); // Y축 90도 회전
			worldMatrix *= XMMatrixTranslation(0.0f, 2.0f, 3.0f);  // X: 5, Z: 2
		}
		else if (i == 7)
		{
			worldMatrix *= XMMatrixScaling(0.08f, 0.08f, 0.08f);    // 크기: 3배
			worldMatrix *= XMMatrixRotationX(XMConvertToRadians(90.0f));
			worldMatrix *= XMMatrixRotationY(rotation * 0.5f);   // Y축 회전 (속도 절반)
			worldMatrix *= XMMatrixTranslation(0.0f, 1.0f, 0.0f);  // z축 0

			// y축으로 ±2 진동
			float yOffset = sinf(rotation) * 1.0f;
			worldMatrix *= XMMatrixTranslation(0.0f, yOffset, 0.0f);
		}
		else if (i == 8)
		{
			worldMatrix *= XMMatrixScaling(0.1f, 0.1f, 0.1f);    // 크기: 3배
			worldMatrix *= XMMatrixRotationX(XMConvertToRadians(90.0f));
			worldMatrix *= XMMatrixRotationY(rotation * 1.0f);   // Y축 회전 (속도 절반)
			worldMatrix *= XMMatrixTranslation(-2.0f, 0.0f, -2.0f);  // X: 5, Z: 2

			float xOffset = sinf(rotation) * 1.5f;
			worldMatrix *= XMMatrixTranslation(xOffset, 0.0f, 0.0f);
		}
		else if (i == 9)
		{
			worldMatrix *= XMMatrixScaling(0.8f, 0.8f, 0.8f);    // 크기: 3배
			//worldMatrix *= XMMatrixRotationX(XMConvertToRadians(90.0f));
			worldMatrix *= XMMatrixRotationY(rotation * 0.3f);   // Y축 회전 (속도 절반)
			worldMatrix *= XMMatrixTranslation(2.0f, 0.5f, -2.0f);  // X: 5, Z: 2

			float zOffset = sinf(rotation) * 2.0f;
			worldMatrix *= XMMatrixTranslation( 0.0f, 0.0f, zOffset);
		}
		else if (i == 5)
		{
			// 인스턴싱 전용 처리
			m_Models[i]->Render(m_D3D->GetDeviceContext());

			// 단위행렬로 지정
			XMMATRIX identityMatrix = XMMatrixIdentity();

			// isInstancing=true로 지정
			m_TextureShader->SetInstancing(true);

			bool result = m_TextureShader->Render(
				m_D3D->GetDeviceContext(),
				m_Models[i]->GetIndexCount(),
				m_Models[i]->GetInstanceCount(),
				identityMatrix, viewMatrix, projectionMatrix,
				m_Models[i]->GetTexture()
			);
			if (!result) return false;

			// 인스턴싱 종료 (다른 모델은 일반)
			m_TextureShader->SetInstancing(false);

			continue;
		}
		else // 나머지 모델은 공통 배치
		{
			float offsetX = (static_cast<float>(i) - (m_Models.size() / 2.0f)) * 3.0f;
			worldMatrix *= XMMatrixScaling(2.0f, 2.0f, 2.0f);
			worldMatrix *= XMMatrixRotationY(rotation);
			worldMatrix *= XMMatrixTranslation(offsetX, 2.0f, 0.0f);
		}

		// 렌더
		m_Models[i]->Render(m_D3D->GetDeviceContext());
		bool result = m_TextureShader->Render(
			m_D3D->GetDeviceContext(),
			m_Models[i]->GetIndexCount(),
			m_Models[i]->GetInstanceCount(),
			worldMatrix, viewMatrix, projectionMatrix,
			m_Models[i]->GetTexture()
		);
		if (!result) return false;
	}



	wchar_t debug[128];
	swprintf(debug, 128, L"Ground IndexCount: %d\n", m_ModelGround->GetIndexCount());
	OutputDebugString(debug);

	// 4. GROUND
	m_D3D->GetWorldMatrix(worldMatrix);
	worldMatrix *= XMMatrixTranslation(0.0f, -0.4f, 0.0f);

	m_ModelGround->Render(m_D3D->GetDeviceContext());
	result = m_TextureShader->Render(
		m_D3D->GetDeviceContext(),
		m_ModelGround->GetIndexCount(),
		m_ModelGround->GetInstanceCount(),
		worldMatrix, viewMatrix, projectionMatrix,
		m_ModelGround->GetTexture()
	);
	if (!result) return false;


	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}

bool GraphicsClass::InitializeSkybox(ID3D11Device* device)
{
	// 2D 풀스크린 Quad 버텍스 (좌하단 기준으로 0,0 ~ 1,1)
	VertexType vertices[] =
	{
		{ XMFLOAT3(-1.0f,  1.0f, 0.999f), XMFLOAT2(0.0f, 0.0f) }, // 좌상단
		{ XMFLOAT3(1.0f,  1.0f, 0.999f), XMFLOAT2(1.0f, 0.0f) }, // 우상단
		{ XMFLOAT3(-1.0f, -1.0f, 0.999f), XMFLOAT2(0.0f, 1.0f) }, // 좌하단
		{ XMFLOAT3(1.0f, -1.0f, 0.999f), XMFLOAT2(1.0f, 1.0f) }, // 우하단
	};

	unsigned long indices[] =
	{
		0, 1, 2,
		2, 1, 3
	};
	m_skyboxIndexCount = 6;

	// 정점 버퍼 생성
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(VertexType) * 4;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vinitData = {};
	vinitData.pSysMem = vertices;

	HRESULT hr = device->CreateBuffer(&vbd, &vinitData, &m_skyboxVertexBuffer);
	if (FAILED(hr)) return false;

	// 인덱스 버퍼 생성
	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(unsigned long) * m_skyboxIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA iinitData = {};
	iinitData.pSysMem = indices;

	hr = device->CreateBuffer(&ibd, &iinitData, &m_skyboxIndexBuffer);
	if (FAILED(hr)) return false;

	// 배경 텍스처 로드
	m_SkyboxModel = new ModelClass;
	if (!m_SkyboxModel) return false;

	bool result = m_SkyboxModel->LoadTexture(device, L"./data/background.dds");
	if (!result) return false;

	m_skyboxTexture = m_SkyboxModel->GetTexture();

	return true;
}

void GraphicsClass::RenderSkybox(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	// 깊이 쓰기 끄기
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	ID3D11DepthStencilState* dsState;
	m_D3D->GetDevice()->CreateDepthStencilState(&dsDesc, &dsState);
	context->OMSetDepthStencilState(dsState, 0);

	// 2D Quad이므로 월드/뷰/프로젝션 행렬을 단위행렬로
	XMMATRIX identityMatrix = XMMatrixIdentity();

	// 정점/인덱스 버퍼 바인딩
	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &m_skyboxVertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(m_skyboxIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 스카이박스 렌더링 (뷰/프로젝션 무시)
	m_TextureShader->Render(context, m_skyboxIndexCount,0,
		identityMatrix, identityMatrix, identityMatrix,
		m_skyboxTexture);

	// 깊이 쓰기 다시 켜기
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	m_D3D->GetDevice()->CreateDepthStencilState(&dsDesc, &dsState);
	context->OMSetDepthStencilState(dsState, 0);

	dsState->Release();
}

bool GraphicsClass::InitializeBillboards(ID3D11Device* device)
{
	// Billboard 메시 (Quad)
	float aspectRatio = 9.0f / 16.0f; // 0.5625

	VertexType vertices[] =
	{
		{ XMFLOAT3(-aspectRatio,  1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // 좌상단
		{ XMFLOAT3(aspectRatio,  1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) }, // 우상단
		{ XMFLOAT3(-aspectRatio, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) }, // 좌하단
		{ XMFLOAT3(aspectRatio, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }, // 우하단
	};

	unsigned long indices[] =
	{
		0, 1, 2,
		2, 1, 3
	};
	m_billboardIndexCount = 6;

	// 정점 버퍼
	D3D11_BUFFER_DESC vbd = { sizeof(VertexType) * 4, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER };
	D3D11_SUBRESOURCE_DATA vinit = { vertices };
	HRESULT hr = device->CreateBuffer(&vbd, &vinit, &m_billboardVertexBuffer);
	if (FAILED(hr)) return false;

	// 인덱스 버퍼
	D3D11_BUFFER_DESC ibd = { sizeof(unsigned long) * m_billboardIndexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER };
	D3D11_SUBRESOURCE_DATA iinit = { indices };
	hr = device->CreateBuffer(&ibd, &iinit, &m_billboardIndexBuffer);
	if (FAILED(hr)) return false;

	// Billboard 1
	ModelClass* tempModel1 = new ModelClass;
	tempModel1->LoadTexture(device, L"./data/billboard2.dds");
	m_billboardTexture1 = tempModel1->GetTexture();
	delete tempModel1;

	// Billboard 2
	ModelClass* tempModel2 = new ModelClass;
	tempModel2->LoadTexture(device, L"./data/billboard2.dds");
	m_billboardTexture2 = tempModel2->GetTexture();
	delete tempModel2;

	return true;
}


void GraphicsClass::RenderBillboards(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	// 예제: 2개의 빌보딩 위치
	XMFLOAT3 positions[] = { XMFLOAT3(5.0f, 2.0f, 3.0f), XMFLOAT3(-3.0f, 2.0f, 5.0f) };
	ID3D11ShaderResourceView* textures[] = { m_billboardTexture1, m_billboardTexture2 };

	for (int i = 0; i < 2; ++i)
	{
		// Billboard 회전 (카메라 회전만 반영)
		XMMATRIX viewRotation = viewMatrix;
		viewRotation.r[3] = XMVectorSet(0, 0, 0, 1);
		viewRotation = XMMatrixTranspose(viewRotation); // 반전
		XMMATRIX translation = XMMatrixTranslation(positions[i].x, positions[i].y, positions[i].z);
		XMMATRIX worldMatrix = XMMatrixMultiply(viewRotation, translation);

		// 정점/인덱스 버퍼
		UINT stride = sizeof(VertexType);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, &m_billboardVertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(m_billboardIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 렌더링
		m_TextureShader->Render(context, m_billboardIndexCount, 0,
			worldMatrix, viewMatrix, projectionMatrix,
			textures[i]);
	}
}

void GraphicsClass::HandleInput()
{
	float speed = 0.5f;

	if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000)
		m_Camera->m_moveBackForward += speed;
	if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000)
		m_Camera->m_moveBackForward -= speed;
	if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000)
		m_Camera->m_moveLeftRight -= speed;
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000)
		m_Camera->m_moveLeftRight += speed;

	// 마우스 회전 처리
	POINT mousePos;
	GetCursorPos(&mousePos);

	static POINT lastMousePos = { 0, 0 };
	if (lastMousePos.x == 0 && lastMousePos.y == 0)
	{
		lastMousePos = mousePos;
	}

	int dx = mousePos.x - lastMousePos.x;
	int dy = mousePos.y - lastMousePos.y;

	m_Camera->m_camYaw += dx * 0.002f;
	m_Camera->m_camPitch += dy * 0.002f;

	lastMousePos = mousePos;
}

void GraphicsClass::SetTimer(TimerClass* timer)
{
	m_Timer = timer;

}

