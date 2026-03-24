cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInputType
{
    float4 position : POSITION;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 texCoord : TEXCOORD0;
};

PixelInputType SkyboxVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4x4 viewNoTranslation;
    
    input.position.w = 1.0f;
    
    // Remove translation from view matrix (skybox follows camera)
    viewNoTranslation = viewMatrix;
    viewNoTranslation[3][0] = 0.0f;
    viewNoTranslation[3][1] = 0.0f;
    viewNoTranslation[3][2] = 0.0f;
    
    // Transform position
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewNoTranslation);
    output.position = mul(output.position, projectionMatrix);
    
    // Set z = w so depth is always 1.0 (farthest)
    output.position.z = output.position.w;
    
    // Use local position as cubemap lookup direction
    output.texCoord = input.position.xyz;
    
    return output;
}