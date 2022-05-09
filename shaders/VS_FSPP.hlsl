struct VSOut
{
    float2 tex : TEXCOORD;
    float4 pos : SV_POSITION;
};

VSOut main(float2 pos : POSITION, float2 tex : TEXCOORD)
{
    VSOut oVS;
    oVS.pos = float4(pos, 0.0, 1.0);
    oVS.tex = tex;
    return oVS;
}