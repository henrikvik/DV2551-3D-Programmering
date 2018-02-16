#define MERGE(a, b) a##b

#ifdef DIFFUSE_SLOT
Texture2D _texture : register(MERGE(t,DIFFUSE_SLOT));
#endif
//SamplerState _sampler : register(s0);

struct cb_t { float4 v; };
ConstantBuffer<cb_t> tint : register(MERGE(b, DIFFUSE_TINT));

struct Pixel
{
    float4 position : SV_POSITION;
    float3 normal : _NORMAL;
    float2 uv : _TEXCOORD;
};

float4 PSMain(Pixel pixel) : SV_TARGET0
{
    float4 color = float4(1, 1, 1, 1);
    #ifdef DIFFUSE_SLOT
    //color *= _texture.Sample(_sampler, pixel.uv);
    #endif
    return color * tint.v;
}