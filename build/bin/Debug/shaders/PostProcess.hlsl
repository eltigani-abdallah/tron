cbuffer PostConstants : register(b0)
{
    float saturation;
    float brightness;
    float contrast;
    float _pad;
};

Texture2D SceneTexture : register(t0);
SamplerState LinearSampler : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

PSInput VSMain(uint vertexId : SV_VertexID)
{
    PSInput output;

    // Fullscreen triangle.
    float2 uv = float2((vertexId << 1) & 2, vertexId & 2);
    output.position = float4(uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    output.uv = uv;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float3 color = SceneTexture.Sample(LinearSampler, input.uv).rgb;

    // Brightness (additive lift).
    color += brightness;

    // Contrast around mid-grey.
    color = (color - 0.5f) * contrast + 0.5f;

    // Saturation via luminance mix.
    float luminance = dot(color, float3(0.2126f, 0.7152f, 0.0722f));
    color = lerp(float3(luminance, luminance, luminance), color, saturation);

    // Subtle cyan lift in shadows + vignette (presentation only).
    color += float3(0.01f, 0.03f, 0.06f) * saturate(0.35f - luminance);
    float2 vc = input.uv * 2.0f - 1.0f;
    float vignette = saturate(1.0f - dot(vc, vc) * 0.18f);
    color *= vignette;

    return float4(saturate(color), 1.0f);
}
