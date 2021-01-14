

Texture2D EquiTexture : register(t0);
RWTexture2DArray<float4> OutputTexture : register(u0);

[numthreads(32, 32, 1)]
void csMain( uint3 DTid : SV_DispatchThreadID )
{
}