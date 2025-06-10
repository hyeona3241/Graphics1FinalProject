#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include "TextureClass.h"

class TitleScreen
{
public:
    TitleScreen();
    ~TitleScreen();

    bool Initialize(ID3D11Device* device, const std::wstring& textureFile);
    void Shutdown();
    void Render(ID3D11DeviceContext* context);



private:
    ID3D11Buffer* m_vertexBuffer;
    ID3D11Buffer* m_indexBuffer;
    
    int m_indexCount;
    TextureClass* m_TextureClass;

public:
    ID3D11ShaderResourceView* m_texture;

};