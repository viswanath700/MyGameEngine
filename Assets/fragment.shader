/*
This is an example of a fragment shader
*/

// Platform-specific setup
#include "shaders.inc"

// Declaration of color uniforms
uniform float3 g_RGBColor;

// Declaration of texture sampler
uniform sampler2D g_textureSampler;

#if defined( EAE6320_PLATFORM_D3D )

// Entry Point
//============

void main(

	// Input
	//======

	// Whatever arbitrary data (i.e. everything excluding position) was output from the vertex shader
	// will be interpolated across the triangle and given as input to the fragment shader

	in float4 i_color : COLOR0,

	in float2 i_texture_coords : TEXCOORD0,

	// Output
	//=======

	// Whatever color value is output from the fragment shader
	// will determine the color of the corresponding pixel on the screen
	out float4 o_color : COLOR0

	)

#elif defined( EAE6320_PLATFORM_GL )

// Input
//======

// Whatever arbitrary data (i.e. everything excluding position) was output from the vertex shader
// will be interpolated across the triangle and given as input to the fragment shader

layout(location = 0) in vec4 i_color;

layout(location = 1) in vec2 i_texture_coords;

// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
out vec4 o_color;

// Entry Point
//============

void main()

#endif
{
	// Set the fragment to the interpolated color that originated as per-vertex data
	// (where color is represented by 4 floats representing "RGBA" == "Red/Green/Blue/Alpha")
	{
		o_color = i_color;
		o_color.rgb *= g_RGBColor;

		o_color *= Texture(g_textureSampler, i_texture_coords);
	}
}
