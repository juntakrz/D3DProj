float4 main(float2 tex : TEXCOORD0) : SV_TARGET
{
    float4 color;
    if(tex.x < 0.007 || tex.y < 0.007 || tex.x > 0.993 || tex.y > 0.993)
    {
        color = float4(1.0, 0.5, 0.0, 1.0);
    }
    else
    {
        color = float4(0.5, 0.5, 0.5, 1.0);
    }
            
    return color;
}