
float3 DoSpeedWave(float3 pos, float intensity)
{
        float3 output = pos;

        // Gerstner Wave
        float dist     = distance(pos.xz, _EyePosition.xz);
        float distWave = sin(-dist / 2.0f + _Time * 2.0f) * 0.5f + 0.5f;

        float alpha = saturate(sqrt(dot(_PlayerVelocity, _PlayerVelocity)) / 5.0f);
        alpha *= distWave;
        alpha *= 1.0f - saturate((dist - 2.0f) / 20.0f);
        output.y -= alpha * intensity ;

        return output;
}