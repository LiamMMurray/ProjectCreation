float3 bezier4(float3 a, float3 b, float3 c, float3 d, float t)
{
        float omt  = 1.0f - t;
        float omt2 = omt * omt;
        float omt3 = omt * omt2;
        float t2   = t * t;
        float t3   = t * t2;
        return omt3 * a + 3.0f * t * omt2 * b + 3.0f * t2 * omt * c + t3 * d;
}

float4 bezier4(float4 a, float4 b, float4 c, float4 d, float t)
{
        float omt  = 1.0f - t;
        float omt2 = omt * omt;
        float omt3 = omt * omt2;
        float t2   = t * t;
        float t3   = t * t2;
        return omt3 * a + 3.0f * t * omt2 * b + 3.0f * t2 * omt * c + t3 * d;
}