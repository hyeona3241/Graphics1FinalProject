// GLOBALS //
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
    int isInstancing;
    float3 padding;
};


Texture2D shaderTexture;
SamplerState SampleType;

// TYPEDEFS //
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 instancePos : TEXCOORD1;
    //float3 instanceScale : TEXCOORD2;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

// Vertex Shader
//PixelInputType TextureVertexShader(VertexInputType input)
//{    
    
//    PixelInputType output;

//	// Change the position vector to be 4 units for proper matrix calculations.
//    input.position.w = 1.0f;

//	// Calculate the position of the vertex against the world, view, and projection matrices.
//    output.position = mul(input.position, worldMatrix);
//    output.position = mul(output.position, viewMatrix);
//    output.position = mul(output.position, projectionMatrix);
    
//	// Store the texture coordinates for the pixel shader.
//    output.tex = input.tex;
    
//    return output;
//}

PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;

    // 기본 위치
    float3 pos = input.position.xyz;

    // 인스턴싱이면 instancePos를 더해줌
    if (isInstancing == 1)
    {
        //pos *= input.instanceScale; // 스케일 먼저!
        pos += input.instancePos; // 위치 적용
    }

    float4 worldPos = float4(pos, 1.0f);

    output.position = mul(worldPos, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.tex = input.tex;

    return output;
}


// Pixel Shader
float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);

    return textureColor;
}


struct VS_OUTPUT
{
    float4 position : SV_POSITION; // 최종 위치
    float2 texcoord : TEXCOORD0; // 텍스처 좌표
};

struct VS_INPUT
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 instancePos : TEXCOORD1;
    float instanceScale : TEXCOORD2;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    // 스케일 적용
    float3 scaledPos = input.position * input.instanceScale;
    float3 worldPos = scaledPos + input.instancePos;

    float4 pos = mul(float4(worldPos, 1.0f), worldMatrix);
    pos = mul(pos, viewMatrix);
    pos = mul(pos, projectionMatrix);

    output.position = pos;
    output.texcoord = input.tex;
    return output;
}