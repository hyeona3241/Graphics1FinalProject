#include "TitleScreen.h"

using namespace DirectX;

TitleScreen::TitleScreen() : m_vertexBuffer(nullptr), m_indexBuffer(nullptr), m_texture(nullptr), m_TextureClass(nullptr), m_indexCount(6) {}

TitleScreen::~TitleScreen() {}

bool TitleScreen::Initialize(ID3D11Device* device, const std::wstring& textureFile)
{
    // Full screen quad vertices
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 tex;
    };

    VertexType vertices[] = {
        { XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }
    };

    unsigned long indices[] = { 0, 1, 2, 2, 1, 3 };

    // Create vertex buffer
    D3D11_BUFFER_DESC vbd = { sizeof(vertices), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER };
    D3D11_SUBRESOURCE_DATA vinitData = { vertices };
    HRESULT hr = device->CreateBuffer(&vbd, &vinitData, &m_vertexBuffer);
    if (FAILED(hr)) return false;

    // Create index buffer
    D3D11_BUFFER_DESC ibd = { sizeof(indices), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER };
    D3D11_SUBRESOURCE_DATA iinitData = { indices };
    hr = device->CreateBuffer(&ibd, &iinitData, &m_indexBuffer);
    if (FAILED(hr)) return false;

    // Load texture
    m_TextureClass = new TextureClass;
    if (!m_TextureClass) return false;
    if (!m_TextureClass->Initialize(device, textureFile.c_str())) return false;
    m_texture = m_TextureClass->GetTexture();

    return true;
}

void TitleScreen::Shutdown()
{
    if (m_TextureClass) { m_TextureClass->Shutdown(); delete m_TextureClass; m_TextureClass = nullptr; }
    if (m_indexBuffer) { m_indexBuffer->Release(); m_indexBuffer = nullptr; }
    if (m_vertexBuffer) { m_vertexBuffer->Release(); m_vertexBuffer = nullptr; }
}

void TitleScreen::Render(ID3D11DeviceContext* context)
{
    UINT stride = sizeof(XMFLOAT3) + sizeof(XMFLOAT2);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 이미지를 풀스크린 Quad로 그리면 됨 (셰이더에서 텍스처만 출력)
    // 실제 그리기는 TextureShader 사용해서 GraphicsClass의 Render에서 처리됨
}
