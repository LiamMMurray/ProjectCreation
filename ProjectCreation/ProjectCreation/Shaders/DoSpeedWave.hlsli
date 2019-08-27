
float3 DoSpeedWave(float3 pos, float intensity)
{
        float3 output = pos;

        float3 v = pos - _EyePosition;
        float3 c = 35.0f * normalize(_PlayerVelocity);

        float  dotcc = dot(c, c);
        float  d     = min(dot(v, c), dotcc) / dot(c, c);
        float3 cp    = _EyePosition + c * d;

        float lineDist = distance(cp, pos);
        // Gerstner Wave

        float dist  = sin(-distance(pos, _EyePosition) / 2.0f + _Time * 2.0f) * 0.5f + 0.5f;
        float ratio = lerp(5.0f, 8.0f, dist);


        float alpha = saturate(sqrt(dot(_PlayerVelocity, _PlayerVelocity)) / 5.0f);
        alpha *= 1.0f - saturate((lineDist - 2.0) / ratio);

        output.y -= alpha * intensity;

        return output;
}