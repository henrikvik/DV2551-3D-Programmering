#define POSITION 0
#define NORMAL 1
#define TEXTCOORD 2
#define TRANSLATION 5
#define TRANSLATION_NAME TranslationBlock
#define DIFFUSE_TINT 6
#define DIFFUSE_TINT_NAME DiffuseColor
#define MERGE(a, b) a##b
StructuredBuffer<float4> positions : register(MERGE(t, POSITION));
StructuredBuffer<float4> normals : register(MERGE(t, NORMAL));
StructuredBuffer<float2> uvs : register(MERGE(t, TEXTCOORD));

struct cb_t { float4 v; };
ConstantBuffer<cb_t> translation : register(MERGE(b, DIFFUSE_TINT));


struct Pixel 
{
    float4 position : SV_POSITION;
    float3 normal : _NORMAL;
    float2 uv : _TEXCOORD;
};

Pixel VSMain(uint vertexId : SV_VERTEXID)
{
    Pixel pixel;
    pixel.position = positions[vertexId] + translation.v;
    pixel.normal   = normals[vertexId].xyz;
    pixel.uv       = uvs[vertexId];
    return pixel;
}