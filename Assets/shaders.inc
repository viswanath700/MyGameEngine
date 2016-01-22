/*
	This file should be #included in every shader
	to set up platform-specific preprocessor directives
	so that the shader itself can be mostly platform-independent
*/

#if defined( EAE6320_PLATFORM_D3D )

// Translate GLSL variable types to HLSL
#define vec2 float2
#define vec3 float3
#define vec4 float4

// Translate GLSL matrices types to HLSL
#define mat2 float2x2
#define mat3 float3x3
#define mat4 float4x4

// Macro to tranform position
#define Transform( i_vector, i_matrix ) mul(i_vector, i_matrix)

// Translate common output screen position variable types to HLSL
#define O_POSITION_SCREEN o_position_screen

// Macro to sample texture
#define Texture( sampler, texcoords ) tex2D( sampler, texcoords )

#elif defined( EAE6320_PLATFORM_GL )

// The version of GLSL to use must come first
#version 330

// This extension is required in order to specify explicit locations for shader inputs and outputs
#extension GL_ARB_separate_shader_objects : require

// Translate HLSL variable types to GLSL
#define float2 vec2
#define float3 vec3
#define float4 vec4

// Translate HLSL matrices types to GLSL
#define float2x2 mat2
#define float3x3 mat3
#define float4x4 mat4

// Macro to tranform position
#define Transform( i_vector, i_matrix ) i_vector * i_matrix

// Translate common output screen position variable types to GLSL
#define O_POSITION_SCREEN gl_Position

// Macro to sample texture
#define Texture( sampler, texcoords ) texture2D( sampler, texcoords )

#endif