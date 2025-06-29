#pragma once

const char* PixelShaderSrcData = R"(
		Texture2D g_Texture : register(t0);
		sampler g_TextureSampler : register(s0);

		struct PS_INPUT
		{
			float4 Position : SV_Position;
			float4 Color : COLOR;
			float2 TexCoord : TEXCOORD0;
		};

		struct PS_OUTPUT
		{
			float4 Color : SV_TARGET;
		};

		PS_OUTPUT ps_main(PS_INPUT input)
		{
			PS_OUTPUT output;
			
			output.Color = g_Texture.Sample(g_TextureSampler, input.TexCoord);
			output.Color = output.Color * input.Color;
			
			return output;
		}

		)";