cbuffer lightCBuf
{
    float3 lightPos;
    float lightIntensity;
    float4 diffColor;
    float4 ambiColor;
};

cbuffer objectCBuf
{
    float4 matColor;
    float specIntensity;
    float specPower;
};

//attenuation curve
static const float attConst = 1.0f;
static const float attLinear = 0.045f;
static const float attQuad = 0.0075f;

float4 main(float3 worldPos : POSITION, float3 normal : NORMAL) : SV_TARGET
{   
    //pixel to light vector data
    const float3 vecToL = lightPos - worldPos;
    const float distanceToL = length(vecToL);
    const float3 directionToL = vecToL / distanceToL;
    
    //diffuse attenuation
    const float att = 1.0f / (attConst + attLinear * distanceToL + attQuad * (distanceToL * distanceToL));
    
    //diffuse  intensity
    const float3 diffuse = diffColor.rgb * lightIntensity * att * max(0.0f, dot(directionToL, normal));
    
    //reflected light vector
    const float3 w = normal * dot(vecToL, normal);
    const float3 vecRefl = w * 2.0f - vecToL;
    
    //calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    const float3 specular = att * (diffColor * lightIntensity) * specIntensity * pow(max(0.0f, dot(normalize(-vecRefl), normalize(worldPos))), specPower);
    
    //final color
    return float4(saturate(diffuse + ambiColor.rgb + specular), 1.0f);
}