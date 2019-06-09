[numthreads(500, 1, 1)]
// input texture
texture2D height : register(t0);
// output buffer
RWBuffer<float> results : register(u0);
void            main(uint3 DTid : SV_DispatchThreadID)
{}