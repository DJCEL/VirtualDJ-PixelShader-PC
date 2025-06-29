////////////////////////////////
// File: PixelShader.hlsl
////////////////////////////////

//--------------------------------------------------------------------------------------
// Textures and Samplers
//--------------------------------------------------------------------------------------
Texture2D g_Texture : register(t0);
sampler g_TextureSampler : register(s0);

//--------------------------------------------------------------------------------------
// Input structure
//--------------------------------------------------------------------------------------
struct PS_INPUT
{
	float4 Position : SV_Position;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD0;
};
//--------------------------------------------------------------------------------------
// Output structure
//--------------------------------------------------------------------------------------
struct PS_OUTPUT
{
    float4 Color : SV_TARGET;
};
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
PS_OUTPUT ps_main(PS_INPUT input)
{
    PS_OUTPUT output;
    float fBlurAmont = 0.25;
    float Center = 0.5;
    float4 rgbaValue = 0;
    float scale = 0;
    int SAMPLECOUNT = 15;
    float DENOM = 14.0;

    input.TexCoord -= Center;
    
    
    for (int i = 0; i < SAMPLECOUNT; i++)
    {
        scale = 1.0 + fBlurAmont * (i / DENOM);
        rgbaValue += g_Texture.Sample(g_TextureSampler, input.TexCoord * scale + Center);
        
    }
    rgbaValue /= SAMPLECOUNT;
    
    output.Color = rgbaValue;
    output.Color = output.Color * input.Color;
    
    return output;
}
