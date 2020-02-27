#pragma pack_matrix( row_major )

#include "mvp.hlsli"

struct VSIn
{
	//uint vertexId : SV_VertexID;
    float3 pos : POSITION;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD;
};

struct VSOut
{
	float4 pos : SV_POSITION;
    float3 uv : TEXCOORD;
    float3 norm : NORMAL;
};


VSOut main(VSIn input)
{
    VSOut output = (VSOut) 0;

    output.pos = float4(input.pos, 1.0f);
    output.uv = input.pos.xyz;
    output.norm = input.norm;

	output.pos = mul( output.pos, modeling);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);

    //output.color = input.color;

	return output;
}
	