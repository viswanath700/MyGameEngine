// Header Files
//=============
#include "../Effect.h"

#include <cassert>
#include <sstream>
#include <gl/GLU.h>
#include "../../Windows/WindowsFunctions.h"
#include "../../UserOutput/UserOutput.h"
#include "../../Math/cMatrix_transformation.h"
#include "../../Math/cQuaternion.h"

// Interface
//==========

enum RenderStates : uint8_t
{
	AlphaTransparency = 1 << 0,
	DepthTest = 1 << 1,
	DepthWrite = 1 << 2,
	FaceCulling = 1 << 3,
};

eae6320::Graphics::Effect::Effect(char* const i_path_effect)
{
	m_programId = 0;

	m_transformViewToScreen = NULL;
	m_transformWorldToView = NULL;
	m_transformViewToScreen = NULL;

	m_path_effect = i_path_effect;
	m_path_vertexShader = NULL;
	m_path_fragmentShader = NULL;

	m_renderStates = NULL;
}

bool eae6320::Graphics::Effect::CreateAndLoadEffect()
{
	if (!ReadFromBinaryEffectFile())
	{
		return false;
	}

	if (!CreateGLProgram())
	{
		return false;
	}

	if (!LoadVertexShader())
	{
		return false;
	}
	if (!LoadFragmentShader())
	{
		return false;
	}

	if (!LinkGLProgram())
	{
		return false;
	}

	m_transformLocalToWorld = glGetUniformLocation(m_programId, "g_transform_localToWorld");
	m_transformWorldToView = glGetUniformLocation(m_programId, "g_transform_worldToView");
	m_transformViewToScreen = glGetUniformLocation(m_programId, "g_transform_viewToScreen");

	return true;
}

void eae6320::Graphics::Effect::SetEffect() {
	// Set the vertex and fragment shaders
	{
		glUseProgram(m_programId);
		assert(glGetError() == GL_NO_ERROR);
	}

	SetRenderStates();
}

void eae6320::Graphics::Effect::SetDrawCallUniforms(eae6320::Math::cMatrix_transformation i_localToWorldTransform, CameraObject i_camera)
{
	Math::cMatrix_transformation i_worldToViewTransform = Math::cMatrix_transformation::CreateWorldToViewTransform(i_camera.m_orientation, i_camera.m_position);
	Math::cMatrix_transformation i_viewToScreenTransform = Math::cMatrix_transformation::CreateViewToScreenTransform(i_camera.m_fieldOfView_y, i_camera.m_aspectRatio, i_camera.m_z_nearPlane, i_camera.m_z_farPlane);

	const GLboolean dontTranspose = false; // Matrices are already in the correct format
	const GLsizei uniformCountToSet = 1;
	glUniformMatrix4fv(m_transformLocalToWorld, uniformCountToSet, dontTranspose, reinterpret_cast<const GLfloat*>(&i_localToWorldTransform));
	glUniformMatrix4fv(m_transformWorldToView, uniformCountToSet, dontTranspose, reinterpret_cast<const GLfloat*>(&i_worldToViewTransform));
	glUniformMatrix4fv(m_transformViewToScreen, uniformCountToSet, dontTranspose, reinterpret_cast<const GLfloat*>(&i_viewToScreenTransform));
}

void eae6320::Graphics::Effect::ReleaseEffect()
{
	if (m_programId != 0)
	{
		glDeleteProgram(m_programId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			std::stringstream errorMessage;
			errorMessage << "OpenGL failed to delete the program: " <<
				reinterpret_cast<const char*>(gluErrorString(errorCode));
			UserOutput::Print(errorMessage.str());
		}
		m_programId = 0;
	}
	//if (m_binaryReadBuffer)
	//{
	//	delete[] m_binaryReadBuffer;
	//	m_binaryReadBuffer = NULL;
	//}
}

bool eae6320::Graphics::Effect::LoadVertexShader() {
	// Verify that compiling shaders at run-time is supported
	{
		GLboolean isShaderCompilingSupported;
		glGetBooleanv(GL_SHADER_COMPILER, &isShaderCompilingSupported);
		if (!isShaderCompilingSupported)
		{
			eae6320::UserOutput::Print("Compiling shaders at run-time isn't supported on this implementation (this should never happen)");
			return false;
		}
	}

	bool wereThereErrors = false;

	// Load the source code from file and set it into a shader
	GLuint vertexShaderId = 0;
	void* shaderSource = NULL;
	{
		// Load the shader source code
		size_t fileSize;
		{
			std::string errorMessage;
			if (!LoadAndAllocateShaderProgram(m_path_vertexShader, shaderSource, fileSize, &errorMessage))
			{
				wereThereErrors = true;
				eae6320::UserOutput::Print(errorMessage);
				goto OnExit;
			}
		}
		// Generate a shader
		vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
		{
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to get an unused vertex shader ID: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				eae6320::UserOutput::Print(errorMessage.str());
				goto OnExit;
			}
			else if (vertexShaderId == 0)
			{
				wereThereErrors = true;
				eae6320::UserOutput::Print("OpenGL failed to get an unused vertex shader ID");
				goto OnExit;
			}
		}
		// Set the source code into the shader
		{
			const GLsizei shaderSourceCount = 1;
			const GLchar* shaderSources[] =
			{
				reinterpret_cast<GLchar*>(shaderSource)
			};
			const GLint* sourcesAreNullTerminated = NULL;
			glShaderSource(vertexShaderId, shaderSourceCount, shaderSources, sourcesAreNullTerminated);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to set the vertex shader source code: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				eae6320::UserOutput::Print(errorMessage.str());
				goto OnExit;
			}
		}
	}
	// Compile the shader source code
	{
		glCompileShader(vertexShaderId);
		GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			// Get compilation info
			// (this won't be used unless compilation fails
			// but it can be useful to look at when debugging)
			std::string compilationInfo;
			{
				GLint infoSize;
				glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoSize);
				errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					sLogInfo info(static_cast<size_t>(infoSize));
					GLsizei* dontReturnLength = NULL;
					glGetShaderInfoLog(vertexShaderId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						compilationInfo = info.memory;
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get compilation info of the vertex shader source code: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						eae6320::UserOutput::Print(errorMessage.str());
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to get the length of the vertex shader compilation info: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					eae6320::UserOutput::Print(errorMessage.str());
					goto OnExit;
				}
			}
			// Check to see if there were compilation errors
			GLint didCompilationSucceed;
			{
				glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &didCompilationSucceed);
				errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					if (didCompilationSucceed == GL_FALSE)
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "The vertex shader failed to compile:\n" << compilationInfo;
						eae6320::UserOutput::Print(errorMessage.str());
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to find out if compilation of the vertex shader source code succeeded: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					eae6320::UserOutput::Print(errorMessage.str());
					goto OnExit;
				}
			}
		}
		else
		{
			wereThereErrors = true;
			std::stringstream errorMessage;
			errorMessage << "OpenGL failed to compile the vertex shader source code: " <<
				reinterpret_cast<const char*>(gluErrorString(errorCode));
			eae6320::UserOutput::Print(errorMessage.str());
			goto OnExit;
		}
	}
	// Attach the shader to the program
	{
		glAttachShader(m_programId, vertexShaderId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			std::stringstream errorMessage;
			errorMessage << "OpenGL failed to attach the vertex shader to the program: " <<
				reinterpret_cast<const char*>(gluErrorString(errorCode));
			eae6320::UserOutput::Print(errorMessage.str());
			goto OnExit;
		}
	}

OnExit:

	if (vertexShaderId != 0)
	{
		// Even if the shader was successfully compiled
		// once it has been attached to the program we can (and should) delete our reference to it
		// (any associated memory that OpenGL has allocated internally will be freed
		// once the program is deleted)
		glDeleteShader(vertexShaderId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			std::stringstream errorMessage;
			errorMessage << "OpenGL failed to delete the vertex shader ID: " <<
				reinterpret_cast<const char*>(gluErrorString(errorCode));
			eae6320::UserOutput::Print(errorMessage.str());
		}
		vertexShaderId = 0;
	}
	if (shaderSource != NULL)
	{
		free(shaderSource);
		shaderSource = NULL;
	}

	return !wereThereErrors;
}

bool eae6320::Graphics::Effect::LoadFragmentShader() {
	// Verify that compiling shaders at run-time is supported
	{
		GLboolean isShaderCompilingSupported;
		glGetBooleanv(GL_SHADER_COMPILER, &isShaderCompilingSupported);
		if (!isShaderCompilingSupported)
		{
			eae6320::UserOutput::Print("Compiling shaders at run-time isn't supported on this implementation (this should never happen)");
			return false;
		}
	}

	bool wereThereErrors = false;

	// Load the source code from file and set it into a shader
	GLuint fragmentShaderId = 0;
	void* shaderSource = NULL;
	{
		// Load the shader source code
		size_t fileSize;
		{
			std::string errorMessage;
			if (!LoadAndAllocateShaderProgram(m_path_fragmentShader, shaderSource, fileSize, &errorMessage))
			{
				wereThereErrors = true;
				eae6320::UserOutput::Print(errorMessage);
				goto OnExit;
			}
		}
		// Generate a shader
		fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
		{
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to get an unused fragment shader ID: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				eae6320::UserOutput::Print(errorMessage.str());
				goto OnExit;
			}
			else if (fragmentShaderId == 0)
			{
				wereThereErrors = true;
				eae6320::UserOutput::Print("OpenGL failed to get an unused fragment shader ID");
				goto OnExit;
			}
		}
		// Set the source code into the shader
		{
			const GLsizei shaderSourceCount = 1;
			const GLchar* shaderSources[] =
			{
				reinterpret_cast<GLchar*>(shaderSource)
			};
			const GLint* sourcesAreNullTerminated = NULL;
			glShaderSource(fragmentShaderId, shaderSourceCount, shaderSources, sourcesAreNullTerminated);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to set the fragment shader source code: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				eae6320::UserOutput::Print(errorMessage.str());
				goto OnExit;
			}
		}
	}
	// Compile the shader source code
	{
		glCompileShader(fragmentShaderId);
		GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			// Get compilation info
			// (this won't be used unless compilation fails
			// but it can be useful to look at when debugging)
			std::string compilationInfo;
			{
				GLint infoSize;
				glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoSize);
				errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					sLogInfo info(static_cast<size_t>(infoSize));
					GLsizei* dontReturnLength = NULL;
					glGetShaderInfoLog(fragmentShaderId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						compilationInfo = info.memory;
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get compilation info of the fragment shader source code: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						eae6320::UserOutput::Print(errorMessage.str());
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to get the length of the fragment shader compilation info: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					eae6320::UserOutput::Print(errorMessage.str());
					goto OnExit;
				}
			}
			// Check to see if there were compilation errors
			GLint didCompilationSucceed;
			{
				glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &didCompilationSucceed);
				errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					if (didCompilationSucceed == GL_FALSE)
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "The fragment shader failed to compile:\n" << compilationInfo;
						eae6320::UserOutput::Print(errorMessage.str());
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to find out if compilation of the fragment shader source code succeeded: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					eae6320::UserOutput::Print(errorMessage.str());
					goto OnExit;
				}
			}
		}
		else
		{
			wereThereErrors = true;
			std::stringstream errorMessage;
			errorMessage << "OpenGL failed to compile the fragment shader source code: " <<
				reinterpret_cast<const char*>(gluErrorString(errorCode));
			eae6320::UserOutput::Print(errorMessage.str());
			goto OnExit;
		}
	}
	// Attach the shader to the program
	{
		glAttachShader(m_programId, fragmentShaderId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			std::stringstream errorMessage;
			errorMessage << "OpenGL failed to attach the fragment shader to the program: " <<
				reinterpret_cast<const char*>(gluErrorString(errorCode));
			eae6320::UserOutput::Print(errorMessage.str());
			goto OnExit;
		}
	}

OnExit:

	if (fragmentShaderId != 0)
	{
		// Even if the shader was successfully compiled
		// once it has been attached to the program we can (and should) delete our reference to it
		// (any associated memory that OpenGL has allocated internally will be freed
		// once the program is deleted)
		glDeleteShader(fragmentShaderId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			std::stringstream errorMessage;
			errorMessage << "OpenGL failed to delete the fragment shader ID: " <<
				reinterpret_cast<const char*>(gluErrorString(errorCode));
			eae6320::UserOutput::Print(errorMessage.str());
		}
		fragmentShaderId = 0;
	}
	if (shaderSource != NULL)
	{
		free(shaderSource);
		shaderSource = NULL;
	}

	return !wereThereErrors;
}

bool eae6320::Graphics::Effect::LoadAndAllocateShaderProgram(const char* i_path, void*& o_shader, size_t& o_size, std::string* o_errorMessage) {
	bool wereThereErrors = false;

	// Load the shader source from disk
	o_shader = NULL;
	HANDLE fileHandle = INVALID_HANDLE_VALUE;
	{
		// Open the file
		{
			const DWORD desiredAccess = FILE_GENERIC_READ;
			const DWORD otherProgramsCanStillReadTheFile = FILE_SHARE_READ;
			SECURITY_ATTRIBUTES* useDefaultSecurity = NULL;
			const DWORD onlySucceedIfFileExists = OPEN_EXISTING;
			const DWORD useDefaultAttributes = FILE_ATTRIBUTE_NORMAL;
			const HANDLE dontUseTemplateFile = NULL;
			fileHandle = CreateFile(i_path, desiredAccess, otherProgramsCanStillReadTheFile,
				useDefaultSecurity, onlySucceedIfFileExists, useDefaultAttributes, dontUseTemplateFile);
			if (fileHandle == INVALID_HANDLE_VALUE)
			{
				wereThereErrors = true;
				if (o_errorMessage)
				{
					std::string windowsErrorMessage = eae6320::GetLastWindowsError();
					std::stringstream errorMessage;
					errorMessage << "Windows failed to open the shader file: " << windowsErrorMessage;
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}
		}
		// Get the file's size
		{
			LARGE_INTEGER fileSize_integer;
			if (GetFileSizeEx(fileHandle, &fileSize_integer) != FALSE)
			{
				assert(fileSize_integer.QuadPart <= SIZE_MAX);
				o_size = static_cast<size_t>(fileSize_integer.QuadPart);
			}
			else
			{
				wereThereErrors = true;
				if (o_errorMessage)
				{
					std::string windowsErrorMessage = eae6320::GetLastWindowsError();
					std::stringstream errorMessage;
					errorMessage << "Windows failed to get the size of shader: " << windowsErrorMessage;
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}
			// Add an extra byte for a NULL terminator
			o_size += 1;
		}
		// Read the file's contents into temporary memory
		o_shader = malloc(o_size);
		if (o_shader)
		{
			DWORD bytesReadCount;
			OVERLAPPED* readSynchronously = NULL;
			if (ReadFile(fileHandle, o_shader, o_size,
				&bytesReadCount, readSynchronously) == FALSE)
			{
				wereThereErrors = true;
				if (o_errorMessage)
				{
					std::string windowsErrorMessage = eae6320::GetLastWindowsError();
					std::stringstream errorMessage;
					errorMessage << "Windows failed to read the contents of shader: " << windowsErrorMessage;
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			if (o_errorMessage)
			{
				std::stringstream errorMessage;
				errorMessage << "Failed to allocate " << o_size << " bytes to read in the shader program " << i_path;
				*o_errorMessage = errorMessage.str();
			}
			goto OnExit;
		}
		// Add the NULL terminator
		reinterpret_cast<char*>(o_shader)[o_size - 1] = '\0';
	}

OnExit:

	if (wereThereErrors && o_shader)
	{
		free(o_shader);
		o_shader = NULL;
	}
	if (fileHandle != INVALID_HANDLE_VALUE)
	{
		if (CloseHandle(fileHandle) == FALSE)
		{
			if (!wereThereErrors && o_errorMessage)
			{
				std::string windowsError = eae6320::GetLastWindowsError();
				std::stringstream errorMessage;
				errorMessage << "Windows failed to close the shader file handle: " << windowsError;
				*o_errorMessage = errorMessage.str();
			}
			wereThereErrors = true;
		}
		fileHandle = INVALID_HANDLE_VALUE;
	}

	return !wereThereErrors;
}

bool eae6320::Graphics::Effect::CreateGLProgram()
{
	// Create a program
	{
		m_programId = glCreateProgram();
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			std::stringstream errorMessage;
			errorMessage << "OpenGL failed to create a program: " <<
				reinterpret_cast<const char*>(gluErrorString(errorCode));
			eae6320::UserOutput::Print(errorMessage.str());
			return false;
		}
		else if (m_programId == 0)
		{
			eae6320::UserOutput::Print("OpenGL failed to create a program");
			return false;
		}

		return true;
	}
}

bool eae6320::Graphics::Effect::LinkGLProgram()
{
	// Link the program
	{
		glLinkProgram(m_programId);
		GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			// Get link info
			// (this won't be used unless linking fails
			// but it can be useful to look at when debugging)
			std::string linkInfo;
			{
				GLint infoSize;
				glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &infoSize);
				errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					sLogInfo info(static_cast<size_t>(infoSize));
					GLsizei* dontReturnLength = NULL;
					glGetProgramInfoLog(m_programId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						linkInfo = info.memory;
					}
					else
					{
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get link info of the program: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						eae6320::UserOutput::Print(errorMessage.str());
						return false;
					}
				}
				else
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to get the length of the program link info: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					eae6320::UserOutput::Print(errorMessage.str());
					return false;
				}
			}
			// Check to see if there were link errors
			GLint didLinkingSucceed;
			{
				glGetProgramiv(m_programId, GL_LINK_STATUS, &didLinkingSucceed);
				errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					if (didLinkingSucceed == GL_FALSE)
					{
						std::stringstream errorMessage;
						errorMessage << "The program failed to link:\n" << linkInfo;
						eae6320::UserOutput::Print(errorMessage.str());
						return false;
					}
				}
				else
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to find out if linking of the program succeeded: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					eae6320::UserOutput::Print(errorMessage.str());
					return false;
				}
			}
		}
		else
		{
			std::stringstream errorMessage;
			errorMessage << "OpenGL failed to link the program: " <<
				reinterpret_cast<const char*>(gluErrorString(errorCode));
			eae6320::UserOutput::Print(errorMessage.str());
			return false;
		}

		return true;
	}
}

void eae6320::Graphics::Effect::SetRenderStates()
{
	// Alpha Transparency
	{
		if ((*m_renderStates) & AlphaTransparency)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}

	// Depth Testing
	{
		if ((*m_renderStates) & DepthTest)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
	}

	// Depth Writing
	{
		if ((*m_renderStates) & DepthWrite)
		{
			glDepthMask(GL_TRUE);
		}
		else
		{
			glDepthMask(GL_FALSE);
		}
	}

	// Face Culling
	{
		if ((*m_renderStates) & FaceCulling)
		{
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
	}
}

GLint eae6320::Graphics::Effect::GetUniformHandle(const char* i_uniformName, bool i_fromVertexShader)
{
	return glGetUniformLocation(m_programId, i_uniformName);
}

void eae6320::Graphics::Effect::SetUniformHandle(sUniformData i_uniformData, bool i_fromVertexShader)
{
	int uniformCount = 1; // We set one uniform at a time.

	switch (i_uniformData.valueCountToSet)
	{
	case 1:
		glUniform1fv(i_uniformData.uniformHandle, uniformCount, i_uniformData.values);
		break;
	case 2:
		glUniform2fv(i_uniformData.uniformHandle, uniformCount, i_uniformData.values);
		break;
	case 3:
		glUniform3fv(i_uniformData.uniformHandle, uniformCount, i_uniformData.values);
		break;
	case 4:
		glUniform4fv(i_uniformData.uniformHandle, uniformCount, i_uniformData.values);
		break;
	default:
		break;
	}

}

GLint eae6320::Graphics::Effect::GetSampler2DID(const char* i_uniformName)
{
	return glGetUniformLocation(m_programId, i_uniformName);
}

void eae6320::Graphics::Effect::SetSampler2DID(Texture::tSampler2D i_sampler, Texture::tTextureHandle i_textureHandle, tTextureUnit i_textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + i_textureUnit);
	glBindTexture(GL_TEXTURE_2D, i_textureHandle);
	glUniform1i(i_sampler, i_textureUnit);
}