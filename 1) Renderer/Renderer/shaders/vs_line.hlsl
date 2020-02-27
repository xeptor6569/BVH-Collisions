#pragma pack_matrix( row_major )

#include "mvp.hlsli"

struct VSIn
{
	//uint vertexId : SV_VertexID;
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct VSOut
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};


VSOut main(VSIn input)
{
    VSOut output = (VSOut) 0;

    output.pos = float4(input.pos, 1.0f);
	output.pos = mul( output.pos, modeling);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);

    output.color = input.color;

	return output;
}
	