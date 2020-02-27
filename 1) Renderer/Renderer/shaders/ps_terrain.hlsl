#pragma pack_matrix( row_major )

Texture2D txDiffuse : register(t0);
SamplerState filter : register(s0);

struct VSOut
{
    float4 pos : SV_POSITION;
    float3 uv : TEXCOORD;
    float3 norm : NORMAL;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
};

PS_OUTPUT main(VSOut input)
{
	PS_OUTPUT output;
    float4 color = { 0.33f, 0.75f, 0.33f, 1.f };

    color.y *= input.norm.y;

    //if ((input.uv.y % 15.6862745) == 0)
    //{
    //    color = float4(0.36f, 0.2824f, 0.1529f, 1.f);
    //}
    //else
    //{
    //    color = float4(0.33f, .75f, 0.33f, 1.f);
    //}
 
    output.color = color; //txDiffuse.Sample(filter, input.uv.xy);

        return output;
    }