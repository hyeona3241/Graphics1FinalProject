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
	//��
	{L"./data/ornate.obj",      L"./data/ornate.dds"},
	//��2
	{L"./data/ornate2.obj", L"./data/ornate2.dds"},
	//����
	{L"./data/statue.obj",       L"./data/statue.dds"},
	//��
	{L"./data/wall.obj",      L"./data/wall.dds"},
	//�ƴ���
	{L"./data/Anubis2.obj",      L"./data/Anubis2.dds"},
	//�ƴ��� ����
	{L"./data/Anubis.obj",      L"./data/Anubis.dds"},
	//����
	{L"./data/ritual.obj", L"./data/ritual.dds"},
	//����ī��
	{L"./data/tstatue.obj",       L"./data/tstatue.dds"},
	//�׾Ƹ�2
	{L"./data/jar2.obj",      L"./data/jar2.dds"},
	//�׾Ƹ�4
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
	m_InstanceModel->SetupInstancing(m_D3D->GetDevice(), 10, startPosition); // 10���� ������, ���� ��ġ

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

	//// �ƴ��� �𵨸� �ν��Ͻ� ����
	//m_Models[5]->SetupInstancing(m_D3D->GetDevice(), 10); // ��: 10�� ����

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
	m_Models.clear(); // ���� ����

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

	//���⼭ �𵨸�
	for (size_t i = 0; i < m_Models.size(); ++i)
	{
		XMMATRIX worldMatrix;
		m_D3D->GetWorldMatrix(worldMatrix);

		if (i == 0) // 0�� �𵨸� Ư���� ��ġ
		{
			worldMatrix *= XMMatrixScaling(2.0f, 2.0f, 2.0f);    // ũ��: 1��
			//worldMatrix *= XMMatrixRotationY(rotation);          // Y�� ȸ��
			worldMatrix *= XMMatrixRotationY(XMConvertToRadians(90.0f)); // Y�� 90�� ȸ��
			worldMatrix *= XMMatrixTranslation(-3.5f, 1.0f, 4.0f); // X: -10���� �̵�
		}
		else if (i == 1) // 1�� �𵨸� �ٸ� ��ġ
		{
			worldMatrix *= XMMatrixScaling(2.0f, 2.0f, 2.0f);    // ũ��: 3��
			//worldMatrix *= XMMatrixRotationY(rotation * 0.5f);   // Y�� ȸ�� (�ӵ� ����)
			worldMatrix *= XMMatrixRotationY(XMConvertToRadians(90.0f)); // Y�� 90�� ȸ��
			worldMatrix *= XMMatrixTranslation(3.5f, 1.0f, 4.0f);  // X: 5, Z: 2
		}
		else if (i == 2)
		{
			worldMatrix *= XMMatrixScaling(1.5f, 1.5f, 1.5f);    // ũ��: 3��
			//worldMatrix *= XMMatrixRotationY(rotation * 0.5f);   // Y�� ȸ�� (�ӵ� ����)
			//worldMatrix *= XMMatrixRotationY(XMConvertToRadians(90.0f)); // Y�� 90�� ȸ��
			worldMatrix *= XMMatrixTranslation(2.0f, 1.5f, 0.5f);  // X: 5, Z: 2
		}
		else if (i == 3)
		{
			worldMatrix *= XMMatrixScaling(5.0f, 5.0f, 5.0f);    // ũ��: 3��
			//worldMatrix *= XMMatrixRotationY(rotation * 0.5f);   // Y�� ȸ�� (�ӵ� ����)
			worldMatrix *= XMMatrixRotationY(XMConvertToRadians(90.0f)); // Y�� 90�� ȸ��
			worldMatrix *= XMMatrixTranslation(0.0f, 2.0f, 15.0f);  // X: 5, Z: 2
		}
		//�ƴ���
		else if (i == 4)
		{
			worldMatrix *= XMMatrixScaling(1.3f, 1.3f, 1.3f);    // ũ��: 3��
			//worldMatrix *= XMMatrixRotationY(rotation * 0.5f);   // Y�� ȸ�� (�ӵ� ����)
			//worldMatrix *= XMMatrixRotationY(XMConvertToRadians(90.0f)); // Y�� 90�� ȸ��
			worldMatrix *= XMMatrixTranslation(-2.0f, 1.3f, 0.5f);  // X: 5, Z: 2
		}
		else if (i == 6)
		{
			worldMatrix *= XMMatrixScaling(2.0f, 2.0f, 2.0f);    // ũ��: 3��
			//worldMatrix *= XMMatrixRotationY(rotation * 0.5f);   // Y�� ȸ�� (�ӵ� ����)
			//worldMatrix *= XMMatrixRotationY(XMConvertToRadians(90.0f)); // Y�� 90�� ȸ��
			worldMatrix *= XMMatrixTranslation(0.0f, 2.0f, 3.0f);  // X: 5, Z: 2
		}
		else if (i == 7)
		{
			worldMatrix *= XMMatrixScaling(0.08f, 0.08f, 0.08f);    // ũ��: 3��
			worldMatrix *= XMMatrixRotationX(XMConvertToRadians(90.0f));
			worldMatrix *= XMMatrixRotationY(rotation * 0.5f);   // Y�� ȸ�� (�ӵ� ����)
			worldMatrix *= XMMatrixTranslation(0.0f, 1.0f, 0.0f);  // z�� 0

			// y������ ��2 ����
			float yOffset = sinf(rotation) * 1.0f;
			worldMatrix *= XMMatrixTranslation(0.0f, yOffset, 0.0f);
		}
		else if (i == 8)
		{
			worldMatrix *= XMMatrixScaling(0.1f, 0.1f, 0.1f);    // ũ��: 3��
			worldMatrix *= XMMatrixRotationX(XMConvertToRadians(90.0f));
			worldMatrix *= XMMatrixRotationY(rotation * 1.0f);   // Y�� ȸ�� (�ӵ� ����)
			worldMatrix *= XMMatrixTranslation(-2.0f, 0.0f, -2.0f);  // X: 5, Z: 2

			float xOffset = sinf(rotation) * 1.5f;
			worldMatrix *= XMMatrixTranslation(xOffset, 0.0f, 0.0f);
		}
		else if (i == 9)
		{
			worldMatrix *= XMMatrixScaling(0.8f, 0.8f, 0.8f);    // ũ��: 3��
			//worldMatrix *= XMMatrixRotationX(XMConvertToRadians(90.0f));
			worldMatrix *= XMMatrixRotationY(rotation * 0.3f);   // Y�� ȸ�� (�ӵ� ����)
			worldMatrix *= XMMatrixTranslation(2.0f, 0.5f, -2.0f);  // X: 5, Z: 2

			float zOffset = sinf(rotation) * 2.0f;
			worldMatrix *= XMMatrixTranslation( 0.0f, 0.0f, zOffset);
		}
		else if (i == 5)
		{
			// �ν��Ͻ� ���� ó��
			m_Models[i]->Render(m_D3D->GetDeviceContext());

			// ������ķ� ����
			XMMATRIX identityMatrix = XMMatrixIdentity();

			// isInstancing=true�� ����
			m_TextureShader->SetInstancing(true);

			bool result = m_TextureShader->Render(
				m_D3D->GetDeviceContext(),
				m_Models[i]->GetIndexCount(),
				m_Models[i]->GetInstanceCount(),
				identityMatrix, viewMatrix, projectionMatrix,
				m_Models[i]->GetTexture()
			);
			if (!result) return false;

			// �ν��Ͻ� ���� (�ٸ� ���� �Ϲ�)
			m_TextureShader->SetInstancing(false);

			continue;
		}
		else // ������ ���� ���� ��ġ
		{
			float offsetX = (static_cast<float>(i) - (m_Models.size() / 2.0f)) * 3.0f;
			worldMatrix *= XMMatrixScaling(2.0f, 2.0f, 2.0f);
			worldMatrix *= XMMatrixRotationY(rotation);
			worldMatrix *= XMMatrixTranslation(offsetX, 2.0f, 0.0f);
		}

		// ����
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
	// 2D Ǯ��ũ�� Quad ���ؽ� (���ϴ� �������� 0,0 ~ 1,1)
	VertexType vertices[] =
	{
		{ XMFLOAT3(-1.0f,  1.0f, 0.999f), XMFLOAT2(0.0f, 0.0f) }, // �»��
		{ XMFLOAT3(1.0f,  1.0f, 0.999f), XMFLOAT2(1.0f, 0.0f) }, // ����
		{ XMFLOAT3(-1.0f, -1.0f, 0.999f), XMFLOAT2(0.0f, 1.0f) }, // ���ϴ�
		{ XMFLOAT3(1.0f, -1.0f, 0.999f), XMFLOAT2(1.0f, 1.0f) }, // ���ϴ�
	};

	unsigned long indices[] =
	{
		0, 1, 2,
		2, 1, 3
	};
	m_skyboxIndexCount = 6;

	// ���� ���� ����
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(VertexType) * 4;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vinitData = {};
	vinitData.pSysMem = vertices;

	HRESULT hr = device->CreateBuffer(&vbd, &vinitData, &m_skyboxVertexBuffer);
	if (FAILED(hr)) return false;

	// �ε��� ���� ����
	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(unsigned long) * m_skyboxIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA iinitData = {};
	iinitData.pSysMem = indices;

	hr = device->CreateBuffer(&ibd, &iinitData, &m_skyboxIndexBuffer);
	if (FAILED(hr)) return false;

	// ��� �ؽ�ó �ε�
	m_SkyboxModel = new ModelClass;
	if (!m_SkyboxModel) return false;

	bool result = m_SkyboxModel->LoadTexture(device, L"./data/background.dds");
	if (!result) return false;

	m_skyboxTexture = m_SkyboxModel->GetTexture();

	return true;
}

void GraphicsClass::RenderSkybox(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	// ���� ���� ����
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	ID3D11DepthStencilState* dsState;
	m_D3D->GetDevice()->CreateDepthStencilState(&dsDesc, &dsState);
	context->OMSetDepthStencilState(dsState, 0);

	// 2D Quad�̹Ƿ� ����/��/�������� ����� ������ķ�
	XMMATRIX identityMatrix = XMMatrixIdentity();

	// ����/�ε��� ���� ���ε�
	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &m_skyboxVertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(m_skyboxIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ��ī�̹ڽ� ������ (��/�������� ����)
	m_TextureShader->Render(context, m_skyboxIndexCount,0,
		identityMatrix, identityMatrix, identityMatrix,
		m_skyboxTexture);

	// ���� ���� �ٽ� �ѱ�
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	m_D3D->GetDevice()->CreateDepthStencilState(&dsDesc, &dsState);
	context->OMSetDepthStencilState(dsState, 0);

	dsState->Release();
}

bool GraphicsClass::InitializeBillboards(ID3D11Device* device)
{
	// Billboard �޽� (Quad)
	float aspectRatio = 9.0f / 16.0f; // 0.5625

	VertexType vertices[] =
	{
		{ XMFLOAT3(-aspectRatio,  1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // �»��
		{ XMFLOAT3(aspectRatio,  1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) }, // ����
		{ XMFLOAT3(-aspectRatio, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) }, // ���ϴ�
		{ XMFLOAT3(aspectRatio, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }, // ���ϴ�
	};

	unsigned long indices[] =
	{
		0, 1, 2,
		2, 1, 3
	};
	m_billboardIndexCount = 6;

	// ���� ����
	D3D11_BUFFER_DESC vbd = { sizeof(VertexType) * 4, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER };
	D3D11_SUBRESOURCE_DATA vinit = { vertices };
	HRESULT hr = device->CreateBuffer(&vbd, &vinit, &m_billboardVertexBuffer);
	if (FAILED(hr)) return false;

	// �ε��� ����
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
	// ����: 2���� ������ ��ġ
	XMFLOAT3 positions[] = { XMFLOAT3(5.0f, 2.0f, 3.0f), XMFLOAT3(-3.0f, 2.0f, 5.0f) };
	ID3D11ShaderResourceView* textures[] = { m_billboardTexture1, m_billboardTexture2 };

	for (int i = 0; i < 2; ++i)
	{
		// Billboard ȸ�� (ī�޶� ȸ���� �ݿ�)
		XMMATRIX viewRotation = viewMatrix;
		viewRotation.r[3] = XMVectorSet(0, 0, 0, 1);
		viewRotation = XMMatrixTranspose(viewRotation); // ����
		XMMATRIX translation = XMMatrixTranslation(positions[i].x, positions[i].y, positions[i].z);
		XMMATRIX worldMatrix = XMMatrixMultiply(viewRotation, translation);

		// ����/�ε��� ����
		UINT stride = sizeof(VertexType);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, &m_billboardVertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(m_billboardIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// ������
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

	// ���콺 ȸ�� ó��
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

