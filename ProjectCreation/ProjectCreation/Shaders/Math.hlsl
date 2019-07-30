float remap(float value, float low1, float high1, float low2, float high2)
{
        return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

float InterleavedGradientNoise(float2 pos)
{
        float3 magic = float3(0.06711056, 0.00583715, 52.9829189);
        return frac(magic.z * frac(dot(pos, magic.xy)));
}

float rand(in float time)
{
        float2 noise = (frac(sin(dot(float2(time, time), float2(12.9898, 78.233) * 2.0)) * 43758.5453));
        return abs(noise.x + noise.y) * 0.5;
}

float rand_1_05(in float2 uv)
{
        float2 noise = (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
        return abs(noise.x + noise.y) * 0.5;
}

float2 rand_2_10(in float2 uv)
{
        float noiseX = (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
        float noiseY = sqrt(1 - noiseX * noiseX);
        return float2(noiseX, noiseY);
}

float2 rand_2_0004(in float2 uv)
{
        float noiseX = (frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453));
        float noiseY = (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
        return float2(noiseX, noiseY) * 0.004;
}

float3 WrapPos(float3 pos, float3 min, float3 max)
{
        return pos - (max - min) * floor(pos / (max - min));
}

float3 WrapPosition(float3 position, float3 Min, float3 Max)
{
        float3 output;
        float3 firstMod;
        float3 secondMod;

        firstMod.xz  = fmod(position.xz - Min.xz, Max.xz - Min.xz);
        secondMod.xz = fmod(Max.xz - Min.xz + firstMod.xz, Max.xz - Min.xz);
        output.xz    = Min.xz + secondMod.xz;
        output.y     = position.y;
        return output;
}

float RandomFloatInRange(inout float seed, float min, float max)
{
        float alpha = rand(seed);
        seed         = alpha;

        float output;
        output = lerp(min, max, alpha);

        return output;
}

float3 RandomFloat3InRange(inout float seed, float3 min, float3 max)
{
        float alphaA = rand(seed);
        float alphaB = rand(alphaA + seed);
        float alphaC = rand(alphaB + alphaA);
        seed         = alphaC;

        float3 output;
        output.x = lerp(min.x, max.x, alphaA);
        output.y = lerp(min.y, max.y, alphaB);
        output.z = lerp(min.z, max.z, alphaC);

        return output;
}
