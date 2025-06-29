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
    int2 pixelCoord = int2(input.Position.xy);

	// Check if the pixel is every other pixel
    if ((pixelCoord.x % 2 == 0) && (pixelCoord.y % 2 == 0))
    {
	// Sample the texture at the given TexCoord coordinates
        output.Color = g_Texture.Sample(g_TextureSampler, input.TexCoord);
    }
    else
    {
	// black for skipped pixels
        output.Color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    output.Color = output.Color * input.Color;
	
    return output;
}
