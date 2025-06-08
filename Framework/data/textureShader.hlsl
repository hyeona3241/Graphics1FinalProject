// GLOBALS //
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

Texture2D shaderTexture;
SamplerState SampleType;

// TYPEDEFS //
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

// Vertex Shader
PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader.
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

//struct VS_INPUT
//{
//    float3 pos : POSITION;
//    float2 tex : TEXCOORD;
//    float3 normal : NORMAL;

//    float3 instPos : TEXCOORD1; // 인스턴싱용 위치
//};

//struct VS_OUTPUT
//{
//    float4 pos : SV_POSITION;
//    float2 tex : TEXCOORD0;
//};

//VS_OUTPUT TextureVertexShader(VS_INPUT input)
//{
//    VS_OUTPUT output;
//    float4 worldPosition = float4(input.pos + input.instPos, 1.0f);
//    output.pos = mul(worldPosition, worldMatrix);
//    output.pos = mul(output.pos, viewMatrix);
//    output.pos = mul(output.pos, projectionMatrix);

//    output.tex = input.tex;
//    return output;
//}
