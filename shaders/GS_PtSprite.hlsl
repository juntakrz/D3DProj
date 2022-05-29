struct GSOut
{
    float2 tex  : TEXCOORD0;
    float1 alpha : TEXCOORD1;
    float4 pos  : SV_POSITION;
};

struct VSOut
{
    float3 pos          : POSITION0;
    float2 scale        : TEXCOORD0;
    matrix transform    : MATRIX0;
};

static const int r = 2;

SamplerComparisonState sComp : register(s1);
Texture2D texDepth : register(t0);

[maxvertexcount(4)]
void main(
	point VSOut iGS[1],
	inout TriangleStream< GSOut > triStream
)
{   
    GSOut oGS;
    
    // convert vertex coords to UV
    float4 posProj = mul(float4(iGS[0].pos, 1.0), iGS[0].transform);
    float3 coords = posProj.xyz / posProj.w;
    
    coords.x = coords.x * 0.5 + 0.5;
    coords.y = 1.0 - (coords.y * 0.5 + 0.5);
    
    float1 step = 0.00012f;
    
    float1 accDepth = 0.0;
    oGS.alpha = 0.0;
    
    if (saturate(coords.x) == coords.x && saturate(coords.y) == coords.y)
    {
        int divider = (r * 2 + 1) * (r * 2 + 1);
        
        for (int x = -r; x <= r; x++)
        {
            for (int y = -r; y <= r; y++)
            {
                accDepth += texDepth.SampleCmpLevelZero(sComp, coords.xy + float2(x * step, y * step), coords.z).r;
            }
        }
        
        accDepth /= divider;
        
        if(accDepth > 0.0)
        {
            float2 scale = iGS[0].scale * accDepth;
            
            float4 v[4];
            v[0] = float4(iGS[0].pos + float3(-scale.x, -scale.y, 0.0), 1.0);
            v[1] = float4(iGS[0].pos + float3(-scale.x, scale.y, 0.0), 1.0);
            v[2] = float4(iGS[0].pos + float3(scale.x, -scale.y, 0.0), 1.0);
            v[3] = float4(iGS[0].pos + float3(scale.x, scale.y, 0.0), 1.0);
    
            float2 gTexC[4] =
            {
                float2(0.0f, 1.0f),
                float2(0.0f, 0.0f),
                float2(1.0f, 1.0f),
                float2(1.0f, 0.0f)
            };
    
            for (uint i = 0; i < 4; i++)
            {
                oGS.pos = mul(v[i], iGS[0].transform);
                oGS.pos.z = 0.0;
        
                oGS.tex = gTexC[i];
                oGS.alpha = accDepth;
        
                triStream.Append(oGS);
            }
        }
    }
}