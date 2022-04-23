cbuffer lightCBuf
{
    float3 lightPos;
    float4 diffColor;
    float4 ambiColor;
    float lightIntensity;
};

cbuffer objectCBuf
{
    float matLightIntensity;
    float specIntensity;
    float specPower;
};

Texture2D tex2D;
SamplerState smplr;

//attenuation curve
static const float attConst = 1.0f;
static const float attLinear = 0.045f;
static const float attQuad = 0.0075f;

float4 main(float3 worldPos : POSITION, float3 normal : NORMAL, float2 tex : TEXCOORD) : SV_TARGET
{   
    //retrieving texture pixel
    float4 colorTex = tex2D.Sample(smplr, tex);
    
    //pixel to light vector data
    float3 vecToL = lightPos - worldPos;
    float distanceToL = length(vecToL);
    float3 directionToL = vecToL / distanceToL;
    
    //diffuse attenuation
    float att = 1.0f / (attConst + attLinear * distanceToL + attQuad * (distanceToL * distanceToL));
    
    //diffuse  intensity
    float3 diffuse = diffColor.rgb * lightIntensity * matLightIntensity * att * max(0.0f, dot(directionToL, normal));
    
    //reflected light vector
    float3 w = normal * dot(vecToL, normal);
    float3 vecRefl = w * 2.0f - vecToL;
    
    //calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    float3 specular = att * (diffColor * lightIntensity) * specIntensity * pow(max(0.0f, dot(normalize(-vecRefl), normalize(worldPos))), specPower);
    
    //final color
    return float4(saturate(diffuse + ambiColor.rgb + specular) * colorTex.rgb, 1.0f);
}