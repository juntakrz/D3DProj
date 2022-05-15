cbuffer M_Material : register(b0)
{
    float4 M_Ambient;
    float4 M_F0;
    float1 M_MatIntensity;
    float1 M_Metalness;
    float1 M_Roughness;
    float1 M_BumpIntensity;
};

cbuffer L_DirLight : register(b1)
{
    float3 L_DirPos;
    float1 L_DirInt;
    float4 L_DirDiffuse;
};

struct PSIn
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 W_Normal : NORMAL0;
    float3 worldPos : POSITION0;
    float4 lightViewPos : POSITION1;
};

Texture2D texAlbedo : register(t0);
Texture2D texDepth : register(t1);
SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap : register(s1);

float4 main(PSIn iPS) : SV_TARGET
{
    // init variables
    float1 bias = 0.001f;
    float4 color = M_Ambient;
    float4 albedo = texAlbedo.Sample(SampleTypeWrap, iPS.tex);
    float2 projTexCoord;
    float1 pixelDepth;
    float1 L_Depth;
    float1 L_DirIntensity;
    
    // light vector
    float3 L = normalize(L_DirPos - iPS.worldPos);
    
    // calculate projected texture coords (sampling depth buffer texture based on light pos)
    projTexCoord.x = iPS.lightViewPos.x / iPS.lightViewPos.w / 2.0f + 0.5f;
    projTexCoord.y = -iPS.lightViewPos.y / iPS.lightViewPos.w / 2.0f + 0.5f;
    
    // if saturated coords are within 0 - 1 range - the pixel is lit
    if(saturate(projTexCoord.x) == projTexCoord.x && saturate(projTexCoord.y) == projTexCoord.y)
    {
        // sample the depth value from depth buffer texture
        pixelDepth = texDepth.Sample(SampleTypeClamp, projTexCoord).r;
        
        // determine if pixel is behind an object
        // calculate light depth and subtract bias to prevent shader precision errors
        L_Depth = (iPS.lightViewPos.z / iPS.lightViewPos.w) - bias;
        
        // compare depth of the depth map value and the depth of the light to determine whether to shadow or to light pixel
        if(L_Depth < pixelDepth)
        {
            // calculate the amount of light for this pixel
            L_DirIntensity = saturate(dot(iPS.W_Normal, normalize(L_DirPos))) * L_DirInt;
            
            if(L_DirIntensity > 0.0f)
            {
                color += color * L_DirIntensity;
                //color = saturate(color);
            }
        }
    }
    
	return color * albedo;
}