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

	XMFLOAT3 startPosition = XMFLOAT3(-5.0f, 5.0f, 1.0f);
	m_Models[5]->SetupInstancing(m_D3D->GetDevice(), 10, startPosition);

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
			worldMatrix *= XMMatrixTranslation(0.0f, 0.0f, 3.0f);  // z축 0
		}
		else if (i == 8)
		{
			worldMatrix *= XMMatrixScaling(0.1f, 0.1f, 0.1f);    // 크기: 3배
			worldMatrix *= XMMatrixRotationX(XMConvertToRadians(90.0f));
			worldMatrix *= XMMatrixRotationY(rotation * 0.5f);   // Y축 회전 (속도 절반)
			worldMatrix *= XMMatrixTranslation(-2.0f, 0.0f, -2.0f);  // X: 5, Z: 2
		}
		else if (i == 9)
		{
			worldMatrix *= XMMatrixScaling(0.8f, 0.8f, 0.8f);    // 크기: 3배
			//worldMatrix *= XMMatrixRotationX(XMConvertToRadians(90.0f));
			worldMatrix *= XMMatrixRotationY(rotation * 0.5f);   // Y축 회전 (속도 절반)
			worldMatrix *= XMMatrixTranslation(2.0f, 0.5f, -2.0f);  // X: 5, Z: 2
		}
		else if (i == 5)
		{
			// Anubis 인스턴싱은 ModelClass에서 알아서 10번 복제 렌더링!
			m_Models[i]->Render(m_D3D->GetDeviceContext());
			bool result = m_TextureShader->Render(
				m_D3D->GetDeviceContext(),
				m_Models[i]->GetIndexCount(),
				XMMatrixIdentity(), viewMatrix, projectionMatrix,
				m_Models[i]->GetTexture()
			);
			if (!result) return false;

			continue; // 아래 worldMatrix 변환/렌더링은 건너뛴다!
		}
		else // 나머지 모델은 공통 배치
		{
			float offsetX = (static_cast<float>(i) - (m_Models.size() / 2.0f)) * 3.0f;
			worldMatrix *= XMMatrixScaling(2.0f, 2.0f, 2.0f);
			worldMatrix *= XMMatrixRotationY(rotation);
			worldMatrix *= XMMatrixTranslation(offsetX, 0.3f, 0.0f);
		}

		// 렌더
		m_Models[i]->Render(m_D3D->GetDeviceContext());
		bool result = m_TextureShader->Render(
			m_D3D->GetDeviceContext(),
			m_Models[i]->GetIndexCount(),
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
		worldMatrix, viewMatrix, projectionMatrix,
		m_ModelGround->GetTexture()
	);
	if (!result) return false;


	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}
