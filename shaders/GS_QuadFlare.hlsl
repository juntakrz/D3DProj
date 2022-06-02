struct GSOut
{
    float2 tex : TEXCOORD0;
    float1 alpha : TEXCOORD1;
    float2 fpos : TEXCOORD2;
    float4 pos : SV_POSITION;
};

struct VSOut
{
    float3 pos : POSITION0;
    matrix transform : MATRIX0;
};

static const int r = 2;

SamplerComparisonState sComp : register(s1);
Texture2D texDepth : register(t0);

[maxvertexcount(4)]
void main(
	point VSOut iGS[1],
	inout TriangleStream<GSOut> triStream
)
{
    GSOut oGS;
    
    // convert vertex 3D coords to texture UV coords
    float4 posProj = mul(float4(iGS[0].pos, 1.0), iGS[0].transform);
    float3 coords = posProj.xyz / posProj.w;  
    coords.x = coords.x * 0.5 + 0.5;
    coords.y = 1.0 - (coords.y * 0.5 + 0.5);
    
    float1 step = 0.00048f;
    
    float1 accDepth = 0.0;
    oGS.alpha = 0.0;
    
    // start point to quad expansion slightly outside of screen space for smoother transition
    if (coords.x > -0.2 && coords.x < 1.2 && coords.y > -0.2 && coords.y < 1.2)
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
        
        if (accDepth > 0.0)
        {            
            float4 v[4];
            v[0] = float4(-1.0, -1.0, 0.0, 1.0);
            v[1] = float4(-1.0, 1.0, 0.0, 1.0);
            v[2] = float4(1.0, -1.0, 0.0, 1.0);
            v[3] = float4(1.0, 1.0, 0.0, 1.0);
    
            float2 gTexC[4] =
            {
                float2(0.0f, 1.0f),
                float2(0.0f, 0.0f),
                float2(1.0f, 1.0f),
                float2(1.0f, 0.0f)
            };
    
            for (uint i = 0; i < 4; i++)
            {
                oGS.pos = v[i];
                
                oGS.fpos = coords.xy;
        
                oGS.tex = gTexC[i];
                oGS.alpha = accDepth;
        
                triStream.Append(oGS);
            }
        }
    }
}