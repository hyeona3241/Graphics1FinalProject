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
    float3 instancePos : TEXCOORD1;
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

    // 인스턴스 오프셋을 더해준다
    float4 worldPos = float4(input.position.xyz + input.instancePos, 1.0f);

    // 월드·뷰·프로젝션
    worldPos = mul(worldPos, worldMatrix);
    worldPos = mul(worldPos, viewMatrix);
    worldPos = mul(worldPos, projectionMatrix);

    output.position = worldPos;
    output.tex = input.tex;
    return output;
    
    
 //   PixelInputType output;

	//// Change the position vector to be 4 units for proper matrix calculations.
 //   input.position.w = 1.0f;

	//// Calculate the position of the vertex against the world, view, and projection matrices.
 //   output.position = mul(input.position, worldMatrix);
 //   output.position = mul(output.position, viewMatrix);
 //   output.position = mul(output.position, projectionMatrix);
    
	//// Store the texture coordinates for the pixel shader.
	//output.tex = input.tex;
    
 //   return output;
}

// Pixel Shader
float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);

    return textureColor;
}

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD0;
    float3 instPos : TEXCOORD1;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;
    float4 worldPos = float4(input.pos + input.instPos, 1.0f);
    output.pos = mul(worldPos, worldMatrix);
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);

    output.tex = input.tex;
    return output;
}
