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
Texture2D texDepth : register(t6);
SamplerState SampleTypeWrap : register(s0);
SamplerState SampleTypeClamp : register(s1);

float1 CalculateShadow(in float4 lightViewPos, in float1 bias)
{
    float3 projCoord;
    float1 projDepth;
    float1 L_Depth;
    float1 shadow = 0.0f;
    float1 texelSize = 0.0007f;  // aspect-incorrect approximation for 1080p

    projCoord.x = lightViewPos.x / lightViewPos.w / 2.0f + 0.5f;
    projCoord.y = -lightViewPos.y / lightViewPos.w / 2.0f + 0.5f;
    
    // if saturated coords are within 0 - 1 range - the pixel is lit
    if (saturate(projCoord.x) == projCoord.x && saturate(projCoord.y) == projCoord.y)
    {
        // determine if pixel is behind an object
        // calculate light depth and subtract bias to prevent shader precision errors
        L_Depth = (lightViewPos.z / lightViewPos.w) - bias;
        
        for (int x = -1; x <= 1; x++)
        {
            for (int y = -1; y <= 1; y++)
            {
        // sample the depth value from depth buffer texture
                projDepth = texDepth.Sample(SampleTypeClamp, projCoord.xy + float2(x, y) * texelSize).r;
        
        // compare depth of the depth map value and the depth of the light to determine whether to shadow or to light pixel
                if (L_Depth < projDepth)
                {
                    shadow += 1.0f;
                }
            }
        }
        
        return shadow / 9.0f;
    }
    else
    {
        return 0.0f;
    }
}

float4 main(PSIn iPS) : SV_TARGET
{
    // init variables
    float1 bias = 0.00001f;
    float1 radiance;
    float1 shadow = 0.0f;
    float4 albedo = texAlbedo.Sample(SampleTypeWrap, iPS.tex);
    
    //calculate directional light intensity
    float1 L_DirIntensity = saturate(dot(iPS.W_Normal, normalize(L_DirPos))) * L_DirInt;
            
    //add directional light
    if (L_DirIntensity > 0.0f)
    {
        radiance = L_DirDiffuse * L_DirIntensity * M_MatIntensity;
    }
    
    shadow = CalculateShadow(iPS.lightViewPos, bias);
    
    return (M_Ambient + radiance * shadow) * albedo;
}