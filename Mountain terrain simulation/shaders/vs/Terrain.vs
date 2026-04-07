cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 color : COLOR;
	float2 tex2 : TEXCOORD1;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float4 color : COLOR;
	float2 tex2 : TEXCOORD1;
	float4 depthPosition : TEXCOORD2;
	float3 worldPosition : TEXCOORD3;
	float4 lightViewPosition : TEXCOORD4;
};

PixelInputType TerrainVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 worldPos;

	input.position.w = 1.0f;

	worldPos = mul(input.position, worldMatrix);
	output.worldPosition = worldPos.xyz;

	output.position = mul(worldPos, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Calculate position from light's perspective for shadow mapping
	output.lightViewPosition = mul(worldPos, lightViewMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

	output.tex = input.tex;
	output.tex2 = input.tex2;

	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	output.tangent = normalize(output.tangent);

	output.binormal = mul(input.binormal, (float3x3)worldMatrix);
	output.binormal = normalize(output.binormal);

	output.color = float4(input.color, 1.0f);

	output.depthPosition = output.position;

	return output;
}