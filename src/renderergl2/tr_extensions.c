/*
===========================================================================
Copyright (C) 2011 James Canete (use.less01@gmail.com)

This file is part of Tremulous.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
// tr_extensions.c - extensions needed by the renderer not in sdl_glimp.c

#ifdef USE_LOCAL_HEADERS
#	include "SDL.h"
#else
#	include <SDL.h>
#endif

#include "tr_local.h"
#include "tr_dsa.h"

// GL_EXT_draw_range_elements
void            (APIENTRY * qglDrawRangeElementsEXT) (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices);

// GL_EXT_multi_draw_arrays
void            (APIENTRY * qglMultiDrawArraysEXT) (GLenum, const GLint *, const GLsizei *, GLsizei);
void            (APIENTRY * qglMultiDrawElementsEXT) (GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei primcount);

// GL_ARB_vertex_shader
void            (APIENTRY * qglBindAttribLocationARB) (GLhandleARB programObj, GLuint index, const GLcharARB * name);
void            (APIENTRY * qglGetActiveAttribARB) (GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei * length,
													GLint * size, GLenum * type, GLcharARB * name);
GLint(APIENTRY * qglGetAttribLocationARB) (GLhandleARB programObj, const GLcharARB * name);

// GL_ARB_vertex_program
void            (APIENTRY * qglVertexAttrib4fARB) (GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
void            (APIENTRY * qglVertexAttrib4fvARB) (GLuint, const GLfloat *);
void            (APIENTRY * qglVertexAttribPointerARB) (GLuint index, GLint size, GLenum type, GLboolean normalized,
														GLsizei stride, const GLvoid * pointer);
void            (APIENTRY * qglEnableVertexAttribArrayARB) (GLuint index);
void            (APIENTRY * qglDisableVertexAttribArrayARB) (GLuint index);

// GL_ARB_vertex_buffer_object
void (APIENTRY * qglBindBufferARB) (GLenum target, GLuint buffer);
void (APIENTRY * qglDeleteBuffersARB) (GLsizei n, const GLuint * buffers);
void (APIENTRY * qglGenBuffersARB) (GLsizei n, GLuint * buffers);

GLboolean(APIENTRY * qglIsBufferARB) (GLuint buffer);
void (APIENTRY * qglBufferDataARB) (GLenum target, GLsizeiptrARB size, const GLvoid * data, GLenum usage);
void (APIENTRY * qglBufferSubDataARB) (GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid * data);
void (APIENTRY * qglGetBufferSubDataARB) (GLenum target, GLintptrARB offset, GLsizeiptrARB size, GLvoid * data);

void (APIENTRY * qglGetBufferParameterivARB) (GLenum target, GLenum pname, GLint * params);
void (APIENTRY * qglGetBufferPointervARB) (GLenum target, GLenum pname, GLvoid * *params);

// GL_ARB_shader_objects
void            (APIENTRY * qglDeleteObjectARB) (GLhandleARB obj);

GLhandleARB(APIENTRY * qglGetHandleARB) (GLenum pname);
void            (APIENTRY * qglDetachObjectARB) (GLhandleARB containerObj, GLhandleARB attachedObj);

GLhandleARB(APIENTRY * qglCreateShaderObjectARB) (GLenum shaderType);
void            (APIENTRY * qglShaderSourceARB) (GLhandleARB shaderObj, GLsizei count, const GLcharARB * *string,
												 const GLint * length);
void            (APIENTRY * qglCompileShaderARB) (GLhandleARB shaderObj);

GLhandleARB(APIENTRY * qglCreateProgramObjectARB) (void);
void            (APIENTRY * qglAttachObjectARB) (GLhandleARB containerObj, GLhandleARB obj);
void            (APIENTRY * qglLinkProgramARB) (GLhandleARB programObj);
void            (APIENTRY * qglUseProgramObjectARB) (GLhandleARB programObj);
void            (APIENTRY * qglValidateProgramARB) (GLhandleARB programObj);
void            (APIENTRY * qglUniform1fARB) (GLint location, GLfloat v0);
void            (APIENTRY * qglUniform2fARB) (GLint location, GLfloat v0, GLfloat v1);
void            (APIENTRY * qglUniform3fARB) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
void            (APIENTRY * qglUniform4fARB) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
void            (APIENTRY * qglUniform1iARB) (GLint location, GLint v0);
void            (APIENTRY * qglUniform2iARB) (GLint location, GLint v0, GLint v1);
void            (APIENTRY * qglUniform3iARB) (GLint location, GLint v0, GLint v1, GLint v2);
void            (APIENTRY * qglUniform4iARB) (GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
void            (APIENTRY * qglUniform1fvARB) (GLint location, GLsizei count, const GLfloat * value);
void            (APIENTRY * qglUniform2fvARB) (GLint location, GLsizei count, const GLfloat * value);
void            (APIENTRY * qglUniform3fvARB) (GLint location, GLsizei count, const GLfloat * value);
void            (APIENTRY * qglUniform4fvARB) (GLint location, GLsizei count, const GLfloat * value);
void            (APIENTRY * qglUniform2ivARB) (GLint location, GLsizei count, const GLint * value);
void            (APIENTRY * qglUniform3ivARB) (GLint location, GLsizei count, const GLint * value);
void            (APIENTRY * qglUniform4ivARB) (GLint location, GLsizei count, const GLint * value);
void            (APIENTRY * qglUniformMatrix2fvARB) (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
void            (APIENTRY * qglUniformMatrix3fvARB) (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
void            (APIENTRY * qglUniformMatrix4fvARB) (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
void            (APIENTRY * qglGetObjectParameterfvARB) (GLhandleARB obj, GLenum pname, GLfloat * params);
void            (APIENTRY * qglGetObjectParameterivARB) (GLhandleARB obj, GLenum pname, GLint * params);
void            (APIENTRY * qglGetInfoLogARB) (GLhandleARB obj, GLsizei maxLength, GLsizei * length, GLcharARB * infoLog);
void            (APIENTRY * qglGetAttachedObjectsARB) (GLhandleARB containerObj, GLsizei maxCount, GLsizei * count,
													   GLhandleARB * obj);
GLint(APIENTRY * qglGetUniformLocationARB) (GLhandleARB programObj, const GLcharARB * name);
void            (APIENTRY * qglGetActiveUniformARB) (GLhandleARB programObj, GLuint index, GLsizei maxIndex, GLsizei * length,
													 GLint * size, GLenum * type, GLcharARB * name);
void            (APIENTRY * qglGetUniformfvARB) (GLhandleARB programObj, GLint location, GLfloat * params);
void            (APIENTRY * qglGetUniformivARB) (GLhandleARB programObj, GLint location, GLint * params);
void            (APIENTRY * qglGetShaderSourceARB) (GLhandleARB obj, GLsizei maxLength, GLsizei * length, GLcharARB * source);

// GL_ARB_texture_compression
void (APIENTRY * qglCompressedTexImage3DARB)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, 
	GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data);
void (APIENTRY * qglCompressedTexImage2DARB)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height,
	GLint border, GLsizei imageSize, const GLvoid *data);
void (APIENTRY * qglCompressedTexImage1DARB)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border,
	GLsizei imageSize, const GLvoid *data);
void (APIENTRY * qglCompressedTexSubImage3DARB)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset,
	GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data);
void (APIENTRY * qglCompressedTexSubImage2DARB)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
	GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
void (APIENTRY * qglCompressedTexSubImage1DARB)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, 
	GLsizei imageSize, const GLvoid *data);
void (APIENTRY * qglGetCompressedTexImageARB)(GLenum target, GLint lod,
	GLvoid *img);

// GL_EXT_framebuffer_object
GLboolean (APIENTRY * qglIsRenderbufferEXT)(GLuint renderbuffer);
void (APIENTRY * qglBindRenderbufferEXT)(GLenum target, GLuint renderbuffer);
void (APIENTRY * qglDeleteRenderbuffersEXT)(GLsizei n, const GLuint *renderbuffers);
void (APIENTRY * qglGenRenderbuffersEXT)(GLsizei n, GLuint *renderbuffers);

void (APIENTRY * qglRenderbufferStorageEXT)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);

void (APIENTRY * qglGetRenderbufferParameterivEXT)(GLenum target, GLenum pname, GLint *params);

GLboolean (APIENTRY * qglIsFramebufferEXT)(GLuint framebuffer);
void (APIENTRY * qglBindFramebufferEXT)(GLenum target, GLuint framebuffer);
void (APIENTRY * qglDeleteFramebuffersEXT)(GLsizei n, const GLuint *framebuffers);
void (APIENTRY * qglGenFramebuffersEXT)(GLsizei n, GLuint *framebuffers);

GLenum (APIENTRY * qglCheckFramebufferStatusEXT)(GLenum target);

void (APIENTRY * qglFramebufferTexture1DEXT)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture,
	GLint level);
void (APIENTRY * qglFramebufferTexture2DEXT)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture,
	GLint level);
void (APIENTRY * qglFramebufferTexture3DEXT)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture,
	GLint level, GLint zoffset);

void (APIENTRY * qglFramebufferRenderbufferEXT)(GLenum target, GLenum attachment, GLenum renderbuffertarget,
	GLuint renderbuffer);

void (APIENTRY * qglGetFramebufferAttachmentParameterivEXT)(GLenum target, GLenum attachment, GLenum pname, GLint *params);

void (APIENTRY * qglGenerateMipmapEXT)(GLenum target);

// GL_ARB_occlusion_query
void (APIENTRY * qglGenQueriesARB)(GLsizei n, GLuint *ids);
void (APIENTRY * qglDeleteQueriesARB)(GLsizei n, const GLuint *ids);
GLboolean (APIENTRY * qglIsQueryARB)(GLuint id);
void (APIENTRY * qglBeginQueryARB)(GLenum target, GLuint id);
void (APIENTRY * qglEndQueryARB)(GLenum target);
void (APIENTRY * qglGetQueryivARB)(GLenum target, GLenum pname, GLint *params);
void (APIENTRY * qglGetQueryObjectivARB)(GLuint id, GLenum pname, GLint *params);
void (APIENTRY * qglGetQueryObjectuivARB)(GLuint id, GLenum pname, GLuint *params);

// GL_EXT_framebuffer_blit
void (APIENTRY * qglBlitFramebufferEXT)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                            GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                            GLbitfield mask, GLenum filter);

// GL_EXT_framebuffer_multisample
void (APIENTRY * qglRenderbufferStorageMultisampleEXT)(GLenum target, GLsizei samples,
	GLenum internalformat, GLsizei width, GLsizei height);

// GL_ARB_draw_buffers
void (APIENTRY * qglDrawBuffersARB)(GLsizei n, const GLenum *bufs);

// GL_ARB_vertex_array_object
void (APIENTRY * qglBindVertexArrayARB)(GLuint array);
void (APIENTRY * qglDeleteVertexArraysARB)(GLsizei n, const GLuint *arrays);
void (APIENTRY * qglGenVertexArraysARB)(GLsizei n, GLuint *arrays);
GLboolean (APIENTRY * qglIsVertexArrayARB)(GLuint array);

// GL_EXT_direct_state_access
GLvoid (APIENTRY * qglBindMultiTexture)(GLenum texunit, GLenum target, GLuint texture);
GLvoid (APIENTRY * qglTextureParameterf)(GLuint texture, GLenum target, GLenum pname, GLfloat param);
GLvoid (APIENTRY * qglTextureParameteri)(GLuint texture, GLenum target, GLenum pname, GLint param);
GLvoid (APIENTRY * qglTextureImage2D)(GLuint texture, GLenum target, GLint level, GLint internalformat,
	GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
GLvoid (APIENTRY * qglTextureSubImage2D)(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset,
	GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
GLvoid (APIENTRY * qglCopyTextureImage2D)(GLuint texture, GLenum target, GLint level, GLenum internalformat,
	GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
GLvoid (APIENTRY * qglCompressedTextureImage2D)(GLuint texture, GLenum target, GLint level, GLenum internalformat,
	GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
GLvoid (APIENTRY * qglCompressedTextureSubImage2D)(GLuint texture, GLenum target, GLint level,
	GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format,
	GLsizei imageSize, const GLvoid *data);
GLvoid (APIENTRY * qglGenerateTextureMipmap)(GLuint texture, GLenum target);

GLvoid(APIENTRY * qglProgramUniform1i)(GLuint program, GLint location, GLint v0);
GLvoid(APIENTRY * qglProgramUniform1f)(GLuint program, GLint location, GLfloat v0);
GLvoid(APIENTRY * qglProgramUniform2f)(GLuint program, GLint location,
	GLfloat v0, GLfloat v1);
GLvoid(APIENTRY * qglProgramUniform3f)(GLuint program, GLint location,
	GLfloat v0, GLfloat v1, GLfloat v2);
GLvoid(APIENTRY * qglProgramUniform4f)(GLuint program, GLint location,
	GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
GLvoid(APIENTRY * qglProgramUniform1fv)(GLuint program, GLint location,
	GLsizei count, const GLfloat *value);
GLvoid(APIENTRY * qglProgramUniformMatrix4fv)(GLuint program, GLint location,
	GLsizei count, GLboolean transpose,
	const GLfloat *value);

GLvoid(APIENTRY * qglNamedRenderbufferStorage)(GLuint renderbuffer,
	GLenum internalformat, GLsizei width, GLsizei height);

GLvoid(APIENTRY * qglNamedRenderbufferStorageMultisample)(GLuint renderbuffer,
	GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);

GLenum(APIENTRY * qglCheckNamedFramebufferStatus)(GLuint framebuffer, GLenum target);
GLvoid(APIENTRY * qglNamedFramebufferTexture2D)(GLuint framebuffer,
	GLenum attachment, GLenum textarget, GLuint texture, GLint level);
GLvoid(APIENTRY * qglNamedFramebufferRenderbuffer)(GLuint framebuffer,
	GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);


static qboolean GLimp_HaveExtension(const char *ext)
{
	const char *ptr = Q_stristr( glConfig.extensions_string, ext );
	if (ptr == NULL)
		return qfalse;
	ptr += strlen(ext);
	return ((*ptr == ' ') || (*ptr == '\0'));  // verify it's complete string.
}

static void *GLimp_GetProcAddressWithFallback(const char *coreName, const char *arbName)
{
	void *proc = NULL;

	if (coreName)
	{
		proc = (void *)SDL_GL_GetProcAddress(coreName);
	}

	if (!proc && arbName)
	{
		proc = (void *)SDL_GL_GetProcAddress(arbName);
	}

	return proc;
}

static GLboolean (APIENTRY * qglIsShaderCore) (GLuint shader);
static GLboolean (APIENTRY * qglIsProgramCore) (GLuint program);
static GLuint (APIENTRY * qglCreateShaderCore) (GLenum type);
static GLuint (APIENTRY * qglCreateProgramCore) (void);
static void (APIENTRY * qglAttachShaderCore) (GLuint program, GLuint shader);
static void (APIENTRY * qglDetachShaderCore) (GLuint program, GLuint shader);
static void (APIENTRY * qglDeleteShaderCore) (GLuint shader);
static void (APIENTRY * qglDeleteProgramCore) (GLuint program);
static void (APIENTRY * qglGetShaderivCore) (GLuint shader, GLenum pname, GLint *params);
static void (APIENTRY * qglGetProgramivCore) (GLuint program, GLenum pname, GLint *params);
static void (APIENTRY * qglGetShaderInfoLogCore) (GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
static void (APIENTRY * qglGetProgramInfoLogCore) (GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
static void (APIENTRY * qglGetAttachedShadersCore) (GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders);

static qboolean GLimp_IsShaderObject(GLhandleARB object)
{
	if (!qglIsShaderCore)
	{
		return qfalse;
	}

	return qglIsShaderCore((GLuint)object) ? qtrue : qfalse;
}

static qboolean GLimp_IsProgramObject(GLhandleARB object)
{
	if (!qglIsProgramCore)
	{
		return qfalse;
	}

	return qglIsProgramCore((GLuint)object) ? qtrue : qfalse;
}

static GLhandleARB APIENTRY GLimp_CreateShaderObjectCompat(GLenum shaderType)
{
	if (!qglCreateShaderCore)
	{
		return 0;
	}

	return (GLhandleARB)qglCreateShaderCore(shaderType);
}

static GLhandleARB APIENTRY GLimp_CreateProgramObjectCompat(void)
{
	if (!qglCreateProgramCore)
	{
		return 0;
	}

	return (GLhandleARB)qglCreateProgramCore();
}

static void APIENTRY GLimp_AttachObjectCompat(GLhandleARB containerObj, GLhandleARB obj)
{
	if (qglAttachShaderCore)
	{
		qglAttachShaderCore((GLuint)containerObj, (GLuint)obj);
	}
}

static void APIENTRY GLimp_DetachObjectCompat(GLhandleARB containerObj, GLhandleARB attachedObj)
{
	if (qglDetachShaderCore)
	{
		qglDetachShaderCore((GLuint)containerObj, (GLuint)attachedObj);
	}
}

static void APIENTRY GLimp_DeleteObjectCompat(GLhandleARB obj)
{
	if (GLimp_IsShaderObject(obj))
	{
		if (qglDeleteShaderCore)
		{
			qglDeleteShaderCore((GLuint)obj);
		}
		return;
	}

	if (GLimp_IsProgramObject(obj))
	{
		if (qglDeleteProgramCore)
		{
			qglDeleteProgramCore((GLuint)obj);
		}
	}
}

static void APIENTRY GLimp_GetObjectParameterivCompat(GLhandleARB obj, GLenum pname, GLint *params)
{
	if (!params)
	{
		return;
	}

	if (GLimp_IsShaderObject(obj))
	{
		if (!qglGetShaderivCore)
		{
			*params = 0;
			return;
		}

		if (pname == GL_OBJECT_TYPE_ARB)
		{
			*params = GL_SHADER_OBJECT_ARB;
		}
		else if (pname == GL_OBJECT_COMPILE_STATUS_ARB || pname == GL_COMPILE_STATUS)
		{
			qglGetShaderivCore((GLuint)obj, GL_COMPILE_STATUS, params);
		}
		else if (pname == GL_OBJECT_INFO_LOG_LENGTH_ARB || pname == GL_INFO_LOG_LENGTH)
		{
			qglGetShaderivCore((GLuint)obj, GL_INFO_LOG_LENGTH, params);
		}
		else if (pname == GL_OBJECT_SHADER_SOURCE_LENGTH_ARB || pname == GL_SHADER_SOURCE_LENGTH)
		{
			qglGetShaderivCore((GLuint)obj, GL_SHADER_SOURCE_LENGTH, params);
		}
		else
		{
			*params = 0;
		}
		return;
	}

	if (GLimp_IsProgramObject(obj))
	{
		if (!qglGetProgramivCore)
		{
			*params = 0;
			return;
		}

		if (pname == GL_OBJECT_TYPE_ARB)
		{
			*params = GL_PROGRAM_OBJECT_ARB;
		}
		else if (pname == GL_OBJECT_LINK_STATUS_ARB || pname == GL_LINK_STATUS)
		{
			qglGetProgramivCore((GLuint)obj, GL_LINK_STATUS, params);
		}
		else if (pname == GL_OBJECT_VALIDATE_STATUS_ARB || pname == GL_VALIDATE_STATUS)
		{
			qglGetProgramivCore((GLuint)obj, GL_VALIDATE_STATUS, params);
		}
		else if (pname == GL_OBJECT_INFO_LOG_LENGTH_ARB || pname == GL_INFO_LOG_LENGTH)
		{
			qglGetProgramivCore((GLuint)obj, GL_INFO_LOG_LENGTH, params);
		}
		else if (pname == GL_OBJECT_ACTIVE_UNIFORMS_ARB || pname == GL_ACTIVE_UNIFORMS)
		{
			qglGetProgramivCore((GLuint)obj, GL_ACTIVE_UNIFORMS, params);
		}
		else if (pname == GL_OBJECT_ATTACHED_OBJECTS_ARB)
		{
			qglGetProgramivCore((GLuint)obj, GL_ATTACHED_SHADERS, params);
		}
		else
		{
			*params = 0;
		}
		return;
	}

	*params = 0;
}

static void APIENTRY GLimp_GetInfoLogCompat(GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog)
{
	if (GLimp_IsShaderObject(obj))
	{
		if (qglGetShaderInfoLogCore)
		{
			qglGetShaderInfoLogCore((GLuint)obj, maxLength, length, (GLchar *)infoLog);
		}
		return;
	}

	if (GLimp_IsProgramObject(obj))
	{
		if (qglGetProgramInfoLogCore)
		{
			qglGetProgramInfoLogCore((GLuint)obj, maxLength, length, (GLchar *)infoLog);
		}
	}
}

static void APIENTRY GLimp_GetAttachedObjectsCompat(GLhandleARB containerObj, GLsizei maxCount, GLsizei *count, GLhandleARB *obj)
{
	if (qglGetAttachedShadersCore)
	{
		qglGetAttachedShadersCore((GLuint)containerObj, maxCount, count, (GLuint *)obj);
	}
}

void GLimp_InitExtraExtensions()
{
	char *extension;
	const char* result[3] = { "...ignoring %s\n", "...using %s\n", "...%s not found\n" };

	// GL_EXT_draw_range_elements
	extension = "GL_EXT_draw_range_elements";
	glRefConfig.drawRangeElements = qfalse;
	qglMultiDrawArraysEXT = NULL;
	qglMultiDrawElementsEXT = NULL;
	if( GLimp_HaveExtension( extension ) )
	{
		qglDrawRangeElementsEXT = (void *) SDL_GL_GetProcAddress("glDrawRangeElementsEXT");

		if ( r_ext_draw_range_elements->integer)
			glRefConfig.drawRangeElements = qtrue;

		ri.Printf(PRINT_ALL, result[glRefConfig.drawRangeElements], extension);
	}
	else
	{
		ri.Printf(PRINT_ALL, result[2], extension);
	}

	// GL_EXT_multi_draw_arrays
	extension = "GL_EXT_multi_draw_arrays";
	glRefConfig.multiDrawArrays = qfalse;
	qglMultiDrawArraysEXT = NULL;
	qglMultiDrawElementsEXT = NULL;
	if( GLimp_HaveExtension( extension ) )
	{
		qglMultiDrawArraysEXT = (PFNGLMULTIDRAWARRAYSEXTPROC) SDL_GL_GetProcAddress("glMultiDrawArraysEXT");
		qglMultiDrawElementsEXT = (PFNGLMULTIDRAWELEMENTSEXTPROC) SDL_GL_GetProcAddress("glMultiDrawElementsEXT");

		if ( r_ext_multi_draw_arrays->integer )
			glRefConfig.multiDrawArrays = qtrue;

		ri.Printf(PRINT_ALL, result[glRefConfig.multiDrawArrays], extension);
	}
	else
	{
		ri.Printf(PRINT_ALL, result[2], extension);
	}

	// GL_ARB_vertex_program
	//glRefConfig.vertexProgram = qfalse;
	extension = "GL_ARB_vertex_program";
	qglVertexAttrib4fARB = NULL;
	qglVertexAttrib4fvARB = NULL;
	qglVertexAttribPointerARB = NULL;
	qglEnableVertexAttribArrayARB = NULL;
	qglDisableVertexAttribArrayARB = NULL;
	if( GLimp_HaveExtension( extension ) )
	{
		qglVertexAttrib4fARB = (PFNGLVERTEXATTRIB4FARBPROC) GLimp_GetProcAddressWithFallback("glVertexAttrib4f", "glVertexAttrib4fARB");
		qglVertexAttrib4fvARB = (PFNGLVERTEXATTRIB4FVARBPROC) GLimp_GetProcAddressWithFallback("glVertexAttrib4fv", "glVertexAttrib4fvARB");
		qglVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARBPROC) GLimp_GetProcAddressWithFallback("glVertexAttribPointer", "glVertexAttribPointerARB");
		qglEnableVertexAttribArrayARB =
			(PFNGLENABLEVERTEXATTRIBARRAYARBPROC) GLimp_GetProcAddressWithFallback("glEnableVertexAttribArray", "glEnableVertexAttribArrayARB");
		qglDisableVertexAttribArrayARB =
			(PFNGLDISABLEVERTEXATTRIBARRAYARBPROC) GLimp_GetProcAddressWithFallback("glDisableVertexAttribArray", "glDisableVertexAttribArrayARB");

		ri.Printf(PRINT_ALL, result[1], extension);
		//glRefConfig.vertexProgram = qtrue;
	}
	else
	{
		ri.Error(ERR_FATAL, result[2], extension);
	}
	
	// GL_ARB_vertex_buffer_object
	//glRefConfig.vertexBufferObject = qfalse;
	extension = "GL_ARB_vertex_buffer_object";
	qglBindBufferARB = NULL;
	qglDeleteBuffersARB = NULL;
	qglGenBuffersARB = NULL;
	qglIsBufferARB = NULL;
	qglBufferDataARB = NULL;
	qglBufferSubDataARB = NULL;
	qglGetBufferSubDataARB = NULL;
	qglGetBufferParameterivARB = NULL;
	qglGetBufferPointervARB = NULL;
	if( GLimp_HaveExtension( extension ) )
	{
		qglBindBufferARB = (PFNGLBINDBUFFERARBPROC) GLimp_GetProcAddressWithFallback("glBindBuffer", "glBindBufferARB");
		qglDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) GLimp_GetProcAddressWithFallback("glDeleteBuffers", "glDeleteBuffersARB");
		qglGenBuffersARB = (PFNGLGENBUFFERSARBPROC) GLimp_GetProcAddressWithFallback("glGenBuffers", "glGenBuffersARB");
		qglIsBufferARB = (PFNGLISBUFFERARBPROC) GLimp_GetProcAddressWithFallback("glIsBuffer", "glIsBufferARB");
		qglBufferDataARB = (PFNGLBUFFERDATAARBPROC) GLimp_GetProcAddressWithFallback("glBufferData", "glBufferDataARB");
		qglBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC) GLimp_GetProcAddressWithFallback("glBufferSubData", "glBufferSubDataARB");
		qglGetBufferSubDataARB = (PFNGLGETBUFFERSUBDATAARBPROC) GLimp_GetProcAddressWithFallback("glGetBufferSubData", "glGetBufferSubDataARB");
		qglGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC) GLimp_GetProcAddressWithFallback("glGetBufferParameteriv", "glGetBufferParameterivARB");
		qglGetBufferPointervARB = (PFNGLGETBUFFERPOINTERVARBPROC) GLimp_GetProcAddressWithFallback("glGetBufferPointerv", "glGetBufferPointervARB");
		ri.Printf(PRINT_ALL, result[1], extension);
		//glRefConfig.vertexBufferObject = qtrue;
	}
	else
	{
		ri.Error(ERR_FATAL, result[2], extension);
	}

	// GL_ARB_shader_objects
	extension = "GL_ARB_shader_objects";
	//glRefConfig.shaderObjects = qfalse;
	qglDeleteObjectARB = NULL;
	qglGetHandleARB = NULL;
	qglDetachObjectARB = NULL;
	qglCreateShaderObjectARB = NULL;
	qglShaderSourceARB = NULL;
	qglCompileShaderARB = NULL;
	qglCreateProgramObjectARB = NULL;
	qglAttachObjectARB = NULL;
	qglLinkProgramARB = NULL;
	qglUseProgramObjectARB = NULL;
	qglValidateProgramARB = NULL;
	qglUniform1fARB = NULL;
	qglUniform2fARB = NULL;
	qglUniform3fARB = NULL;
	qglUniform4fARB = NULL;
	qglUniform1iARB = NULL;
	qglUniform2iARB = NULL;
	qglUniform3iARB = NULL;
	qglUniform4iARB = NULL;
	qglUniform1fvARB = NULL;
	qglUniform2fvARB = NULL;
	qglUniform3fvARB = NULL;
	qglUniform4fvARB = NULL;
	qglUniform2ivARB = NULL;
	qglUniform3ivARB = NULL;
	qglUniform4ivARB = NULL;
	qglUniformMatrix2fvARB = NULL;
	qglUniformMatrix3fvARB = NULL;
	qglUniformMatrix4fvARB = NULL;
	qglGetObjectParameterfvARB = NULL;
	qglGetObjectParameterivARB = NULL;
	qglGetInfoLogARB = NULL;
	qglGetAttachedObjectsARB = NULL;
	qglGetUniformLocationARB = NULL;
	qglGetActiveUniformARB = NULL;
	qglGetUniformfvARB = NULL;
	qglGetUniformivARB = NULL;
	qglGetShaderSourceARB = NULL;

	qglIsShaderCore = (void *)SDL_GL_GetProcAddress("glIsShader");
	qglIsProgramCore = (void *)SDL_GL_GetProcAddress("glIsProgram");
	qglCreateShaderCore = (void *)SDL_GL_GetProcAddress("glCreateShader");
	qglCreateProgramCore = (void *)SDL_GL_GetProcAddress("glCreateProgram");
	qglAttachShaderCore = (void *)SDL_GL_GetProcAddress("glAttachShader");
	qglDetachShaderCore = (void *)SDL_GL_GetProcAddress("glDetachShader");
	qglDeleteShaderCore = (void *)SDL_GL_GetProcAddress("glDeleteShader");
	qglDeleteProgramCore = (void *)SDL_GL_GetProcAddress("glDeleteProgram");
	qglGetShaderivCore = (void *)SDL_GL_GetProcAddress("glGetShaderiv");
	qglGetProgramivCore = (void *)SDL_GL_GetProcAddress("glGetProgramiv");
	qglGetShaderInfoLogCore = (void *)SDL_GL_GetProcAddress("glGetShaderInfoLog");
	qglGetProgramInfoLogCore = (void *)SDL_GL_GetProcAddress("glGetProgramInfoLog");
	qglGetAttachedShadersCore = (void *)SDL_GL_GetProcAddress("glGetAttachedShaders");

	if( GLimp_HaveExtension( extension ) || (qglCreateShaderCore && qglCreateProgramCore) )
	{
		qglDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC) SDL_GL_GetProcAddress("glDeleteObjectARB");
		qglGetHandleARB = (PFNGLGETHANDLEARBPROC) SDL_GL_GetProcAddress("glGetHandleARB");
		qglDetachObjectARB = (PFNGLDETACHOBJECTARBPROC) SDL_GL_GetProcAddress("glDetachObjectARB");
		qglCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC) SDL_GL_GetProcAddress("glCreateShaderObjectARB");
		qglShaderSourceARB = (PFNGLSHADERSOURCEARBPROC) GLimp_GetProcAddressWithFallback("glShaderSource", "glShaderSourceARB");
		qglCompileShaderARB = (PFNGLCOMPILESHADERARBPROC) GLimp_GetProcAddressWithFallback("glCompileShader", "glCompileShaderARB");
		qglCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC) SDL_GL_GetProcAddress("glCreateProgramObjectARB");
		qglAttachObjectARB = (PFNGLATTACHOBJECTARBPROC) SDL_GL_GetProcAddress("glAttachObjectARB");
		qglLinkProgramARB = (PFNGLLINKPROGRAMARBPROC) GLimp_GetProcAddressWithFallback("glLinkProgram", "glLinkProgramARB");
		qglUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC) GLimp_GetProcAddressWithFallback("glUseProgram", "glUseProgramObjectARB");
		qglValidateProgramARB = (PFNGLVALIDATEPROGRAMARBPROC) GLimp_GetProcAddressWithFallback("glValidateProgram", "glValidateProgramARB");
		qglUniform1fARB = (PFNGLUNIFORM1FARBPROC) GLimp_GetProcAddressWithFallback("glUniform1f", "glUniform1fARB");
		qglUniform2fARB = (PFNGLUNIFORM2FARBPROC) GLimp_GetProcAddressWithFallback("glUniform2f", "glUniform2fARB");
		qglUniform3fARB = (PFNGLUNIFORM3FARBPROC) GLimp_GetProcAddressWithFallback("glUniform3f", "glUniform3fARB");
		qglUniform4fARB = (PFNGLUNIFORM4FARBPROC) GLimp_GetProcAddressWithFallback("glUniform4f", "glUniform4fARB");
		qglUniform1iARB = (PFNGLUNIFORM1IARBPROC) GLimp_GetProcAddressWithFallback("glUniform1i", "glUniform1iARB");
		qglUniform2iARB = (PFNGLUNIFORM2IARBPROC) GLimp_GetProcAddressWithFallback("glUniform2i", "glUniform2iARB");
		qglUniform3iARB = (PFNGLUNIFORM3IARBPROC) GLimp_GetProcAddressWithFallback("glUniform3i", "glUniform3iARB");
		qglUniform4iARB = (PFNGLUNIFORM4IARBPROC) GLimp_GetProcAddressWithFallback("glUniform4i", "glUniform4iARB");
		qglUniform1fvARB = (PFNGLUNIFORM1FVARBPROC) GLimp_GetProcAddressWithFallback("glUniform1fv", "glUniform1fvARB");
		qglUniform2fvARB = (PFNGLUNIFORM2FVARBPROC) GLimp_GetProcAddressWithFallback("glUniform2fv", "glUniform2fvARB");
		qglUniform3fvARB = (PFNGLUNIFORM3FVARBPROC) GLimp_GetProcAddressWithFallback("glUniform3fv", "glUniform3fvARB");
		qglUniform4fvARB = (PFNGLUNIFORM4FVARBPROC) GLimp_GetProcAddressWithFallback("glUniform4fv", "glUniform4fvARB");
		qglUniform2ivARB = (PFNGLUNIFORM2IVARBPROC) GLimp_GetProcAddressWithFallback("glUniform2iv", "glUniform2ivARB");
		qglUniform3ivARB = (PFNGLUNIFORM3IVARBPROC) GLimp_GetProcAddressWithFallback("glUniform3iv", "glUniform3ivARB");
		qglUniform4ivARB = (PFNGLUNIFORM4IVARBPROC) GLimp_GetProcAddressWithFallback("glUniform4iv", "glUniform4ivARB");
		qglUniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVARBPROC) GLimp_GetProcAddressWithFallback("glUniformMatrix2fv", "glUniformMatrix2fvARB");
		qglUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC) GLimp_GetProcAddressWithFallback("glUniformMatrix3fv", "glUniformMatrix3fvARB");
		qglUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC) GLimp_GetProcAddressWithFallback("glUniformMatrix4fv", "glUniformMatrix4fvARB");
		qglGetObjectParameterfvARB = (PFNGLGETOBJECTPARAMETERFVARBPROC) SDL_GL_GetProcAddress("glGetObjectParameterfvARB");
		qglGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC) SDL_GL_GetProcAddress("glGetObjectParameterivARB");
		qglGetInfoLogARB = (PFNGLGETINFOLOGARBPROC) SDL_GL_GetProcAddress("glGetInfoLogARB");
		qglGetAttachedObjectsARB = (PFNGLGETATTACHEDOBJECTSARBPROC) SDL_GL_GetProcAddress("glGetAttachedObjectsARB");
		qglGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC) GLimp_GetProcAddressWithFallback("glGetUniformLocation", "glGetUniformLocationARB");
		qglGetActiveUniformARB = (PFNGLGETACTIVEUNIFORMARBPROC) GLimp_GetProcAddressWithFallback("glGetActiveUniform", "glGetActiveUniformARB");
		qglGetUniformfvARB = (PFNGLGETUNIFORMFVARBPROC) GLimp_GetProcAddressWithFallback("glGetUniformfv", "glGetUniformfvARB");
		qglGetUniformivARB = (PFNGLGETUNIFORMIVARBPROC) GLimp_GetProcAddressWithFallback("glGetUniformiv", "glGetUniformivARB");
		qglGetShaderSourceARB = (PFNGLGETSHADERSOURCEARBPROC) GLimp_GetProcAddressWithFallback("glGetShaderSource", "glGetShaderSourceARB");

		if (!qglCreateShaderObjectARB && qglCreateShaderCore)
		{
			qglCreateShaderObjectARB = GLimp_CreateShaderObjectCompat;
		}
		if (!qglCreateProgramObjectARB && qglCreateProgramCore)
		{
			qglCreateProgramObjectARB = GLimp_CreateProgramObjectCompat;
		}
		if (!qglAttachObjectARB && qglAttachShaderCore)
		{
			qglAttachObjectARB = GLimp_AttachObjectCompat;
		}
		if (!qglDetachObjectARB && qglDetachShaderCore)
		{
			qglDetachObjectARB = GLimp_DetachObjectCompat;
		}
		if (!qglDeleteObjectARB && (qglDeleteShaderCore || qglDeleteProgramCore))
		{
			qglDeleteObjectARB = GLimp_DeleteObjectCompat;
		}
		if (!qglGetObjectParameterivARB && (qglGetShaderivCore || qglGetProgramivCore))
		{
			qglGetObjectParameterivARB = GLimp_GetObjectParameterivCompat;
		}
		if (!qglGetInfoLogARB && (qglGetShaderInfoLogCore || qglGetProgramInfoLogCore))
		{
			qglGetInfoLogARB = GLimp_GetInfoLogCompat;
		}
		if (!qglGetAttachedObjectsARB && qglGetAttachedShadersCore)
		{
			qglGetAttachedObjectsARB = GLimp_GetAttachedObjectsCompat;
		}

		if (!qglCreateShaderObjectARB || !qglShaderSourceARB || !qglCompileShaderARB ||
			!qglCreateProgramObjectARB || !qglAttachObjectARB || !qglLinkProgramARB ||
			!qglUseProgramObjectARB || !qglValidateProgramARB || !qglGetObjectParameterivARB ||
			!qglGetInfoLogARB || !qglGetUniformLocationARB || !qglGetActiveUniformARB)
		{
			ri.Error(ERR_FATAL, result[2], extension);
		}
		ri.Printf(PRINT_ALL, result[1], extension);
		//glRefConfig.shaderObjects = qtrue;
	}
	else
	{
		ri.Error(ERR_FATAL, result[2], extension);
	}

	// GL_ARB_vertex_shader
	//glRefConfig.vertexShader = qfalse;
	extension = "GL_ARB_vertex_shader";
	qglBindAttribLocationARB = NULL;
	qglGetActiveAttribARB = NULL;
	qglGetAttribLocationARB = NULL;
	if( GLimp_HaveExtension( extension ) || SDL_GL_GetProcAddress("glBindAttribLocation") )
	{
		//int				reservedComponents;

		//qglGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB, &glConfig.maxVertexUniforms);
		//qglGetIntegerv(GL_MAX_VARYING_FLOATS_ARB, &glConfig.maxVaryingFloats);
		//qglGetIntegerv(GL_MAX_VERTEX_ATTRIBS_ARB, &glConfig.maxVertexAttribs);

		//reservedComponents = 16 * 10; // approximation how many uniforms we have besides the bone matrices

#if 0
		if(glConfig.driverType == GLDRV_MESA)
		{
			// HACK
			// restrict to number of vertex uniforms to 512 because of:
			// xreal.x86_64: nv50_program.c:4181: nv50_program_validate_data: Assertion `p->param_nr <= 512' failed

			glConfig.maxVertexUniforms = Q_bound(0, glConfig.maxVertexUniforms, 512);
		}
#endif

		//glConfig.maxVertexSkinningBones = (int) Q_bound(0.0, (Q_max(glConfig.maxVertexUniforms - reservedComponents, 0) / 16), MAX_BONES);
		//glConfig.vboVertexSkinningAvailable = r_vboVertexSkinning->integer && ((glConfig.maxVertexSkinningBones >= 12) ? qtrue : qfalse);

		qglBindAttribLocationARB = (PFNGLBINDATTRIBLOCATIONARBPROC) GLimp_GetProcAddressWithFallback("glBindAttribLocation", "glBindAttribLocationARB");
		qglGetActiveAttribARB = (PFNGLGETACTIVEATTRIBARBPROC) GLimp_GetProcAddressWithFallback("glGetActiveAttrib", "glGetActiveAttribARB");
		qglGetAttribLocationARB = (PFNGLGETATTRIBLOCATIONARBPROC) GLimp_GetProcAddressWithFallback("glGetAttribLocation", "glGetAttribLocationARB");
		ri.Printf(PRINT_ALL, result[1], extension);
		//glRefConfig.vertexShader = qtrue;
	}
	else
	{
		ri.Error(ERR_FATAL, result[2], extension);
	}

	// GL_ARB_shading_language_100
	extension = "GL_ARB_shading_language_100";
	glRefConfig.textureFloat = qfalse;
	if( GLimp_HaveExtension( extension ) )
	{
		const GLubyte *glslVersionString;
		char version[256];

		glslVersionString = qglGetString(GL_SHADING_LANGUAGE_VERSION);
		if (!glslVersionString)
		{
			glslVersionString = qglGetString(GL_SHADING_LANGUAGE_VERSION_ARB);
		}

		if (!glslVersionString)
		{
			ri.Error(ERR_FATAL, "OpenGL driver did not return a GLSL version string");
		}

		Q_strncpyz(version, (const char *)glslVersionString, sizeof(version));

		sscanf(version, "%d.%d", &glRefConfig.glslMajorVersion, &glRefConfig.glslMinorVersion);

		ri.Printf(PRINT_ALL, "...using GLSL version %s\n", version);
	}
	else
	{
		ri.Error(ERR_FATAL, result[2], extension);
	}

	glRefConfig.memInfo = MI_NONE;

	if( GLimp_HaveExtension( "GL_NVX_gpu_memory_info" ) )
	{
		glRefConfig.memInfo = MI_NVX;
	}
	else if( GLimp_HaveExtension( "GL_ATI_meminfo" ) )
	{
		glRefConfig.memInfo = MI_ATI;
	}

	extension = "GL_ARB_texture_non_power_of_two";
	glRefConfig.textureNonPowerOfTwo = qfalse;
	if( GLimp_HaveExtension( extension ) )
	{
		if(1) //(r_ext_texture_non_power_of_two->integer)
		{
			glRefConfig.textureNonPowerOfTwo = qtrue;
		}

		ri.Printf(PRINT_ALL, result[glRefConfig.textureNonPowerOfTwo], extension);
	}
	else
	{
		ri.Printf(PRINT_ALL, result[2], extension);
	}

	// GL_ARB_texture_float
	extension = "GL_ARB_texture_float";
	glRefConfig.textureFloat = qfalse;
	if( GLimp_HaveExtension( extension ) )
	{
		if( r_ext_texture_float->integer )
		{
			glRefConfig.textureFloat = qtrue;
		}

		ri.Printf(PRINT_ALL, result[glRefConfig.textureFloat], extension);
	}
	else
	{
		ri.Printf(PRINT_ALL, result[2], extension);
	}

	// GL_ARB_half_float_pixel
	extension = "GL_ARB_half_float_pixel";
	glRefConfig.halfFloatPixel = qfalse;
	if( GLimp_HaveExtension( extension ) )
	{
		if( r_arb_half_float_pixel->integer )
			glRefConfig.halfFloatPixel = qtrue;

		ri.Printf(PRINT_ALL, result[glRefConfig.halfFloatPixel], extension);
	}
	else
	{
		ri.Printf(PRINT_ALL, result[2], extension);
	}

	// GL_EXT_framebuffer_object
	extension = "GL_EXT_framebuffer_object";
	glRefConfig.framebufferObject = qfalse;
	if( GLimp_HaveExtension( extension ) )
	{
		glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE_EXT, &glRefConfig.maxRenderbufferSize);
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &glRefConfig.maxColorAttachments);

		qglIsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC) SDL_GL_GetProcAddress("glIsRenderbufferEXT");
		qglBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC) SDL_GL_GetProcAddress("glBindRenderbufferEXT");
		qglDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC) SDL_GL_GetProcAddress("glDeleteRenderbuffersEXT");
		qglGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC) SDL_GL_GetProcAddress("glGenRenderbuffersEXT");
		qglRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC) SDL_GL_GetProcAddress("glRenderbufferStorageEXT");
		qglGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC) SDL_GL_GetProcAddress("glGetRenderbufferParameterivEXT");
		qglIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC) SDL_GL_GetProcAddress("glIsFramebufferEXT");
		qglBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC) SDL_GL_GetProcAddress("glBindFramebufferEXT");
		qglDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC) SDL_GL_GetProcAddress("glDeleteFramebuffersEXT");
		qglGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC) SDL_GL_GetProcAddress("glGenFramebuffersEXT");
		qglCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) SDL_GL_GetProcAddress("glCheckFramebufferStatusEXT");
		qglFramebufferTexture1DEXT = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC) SDL_GL_GetProcAddress("glFramebufferTexture1DEXT");
		qglFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) SDL_GL_GetProcAddress("glFramebufferTexture2DEXT");
		qglFramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC) SDL_GL_GetProcAddress("glFramebufferTexture3DEXT");
		qglFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) SDL_GL_GetProcAddress("glFramebufferRenderbufferEXT");
		qglGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC) SDL_GL_GetProcAddress("glGetFramebufferAttachmentParameterivEXT");
		qglGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC) SDL_GL_GetProcAddress("glGenerateMipmapEXT");

		if(r_ext_framebuffer_object->value)
			glRefConfig.framebufferObject = qtrue;

		ri.Printf(PRINT_ALL, result[glRefConfig.framebufferObject], extension);
	}
	else
	{
		ri.Printf(PRINT_ALL, result[2], extension);
	}

	// GL_EXT_packed_depth_stencil
	extension = "GL_EXT_packed_depth_stencil";
	glRefConfig.packedDepthStencil = qfalse;
	if( GLimp_HaveExtension(extension))
	{
		glRefConfig.packedDepthStencil = qtrue;
		ri.Printf(PRINT_ALL, result[glRefConfig.packedDepthStencil], extension);
	}
	else
	{
		ri.Printf(PRINT_ALL, result[2], extension);
	}

	// GL_ARB_occlusion_query
	extension = "GL_ARB_occlusion_query";
	glRefConfig.occlusionQuery = qfalse;
	if (GLimp_HaveExtension(extension))
	{
		qglGenQueriesARB = (PFNGLGENQUERIESARBPROC) SDL_GL_GetProcAddress("glGenQueriesARB");
		qglDeleteQueriesARB = (PFNGLDELETEQUERIESARBPROC) SDL_GL_GetProcAddress("glDeleteQueriesARB");
		qglIsQueryARB = (PFNGLISQUERYARBPROC) SDL_GL_GetProcAddress("glIsQueryARB");
		qglBeginQueryARB = (PFNGLBEGINQUERYARBPROC) SDL_GL_GetProcAddress("glBeginQueryARB");
		qglEndQueryARB = (PFNGLENDQUERYARBPROC) SDL_GL_GetProcAddress("glEndQueryARB");
		qglGetQueryivARB = (PFNGLGETQUERYIVARBPROC) SDL_GL_GetProcAddress("glGetQueryivARB");
		qglGetQueryObjectivARB = (PFNGLGETQUERYOBJECTIVARBPROC) SDL_GL_GetProcAddress("glGetQueryObjectivARB");
		qglGetQueryObjectuivARB = (PFNGLGETQUERYOBJECTUIVARBPROC) SDL_GL_GetProcAddress("glGetQueryObjectuivARB");
		glRefConfig.occlusionQuery = qtrue;
		ri.Printf(PRINT_ALL, result[glRefConfig.occlusionQuery], extension);
	}
	else
	{
		ri.Printf(PRINT_ALL, result[2], extension);
	}

	// GL_EXT_framebuffer_blit
	extension = "GL_EXT_framebuffer_blit";
	glRefConfig.framebufferBlit = qfalse;
	if (GLimp_HaveExtension(extension))
	{
		qglBlitFramebufferEXT = (void *)SDL_GL_GetProcAddress("glBlitFramebufferEXT");
		glRefConfig.framebufferBlit = qtrue;
		ri.Printf(PRINT_ALL, result[glRefConfig.framebufferBlit], extension);
	}
	else
	{
		ri.Printf(PRINT_ALL, result[2], extension);
	}

	// GL_ARB_texture_compression
	extension = "GL_ARB_texture_compression";
	glRefConfig.arbTextureCompression = qfalse;
	if (GLimp_HaveExtension(extension))
	{
		qglCompressedTexImage3DARB = (void *)SDL_GL_GetProcAddress("glCompressedTexImage3DARB");
		qglCompressedTexImage2DARB = (void *)SDL_GL_GetProcAddress("glCompressedTexImage2DARB");
		qglCompressedTexImage1DARB = (void *)SDL_GL_GetProcAddress("glCompressedTexImage1DARB");
		qglCompressedTexSubImage3DARB = (void *)SDL_GL_GetProcAddress("glCompressedTexSubImage3DARB");
		qglCompressedTexSubImage2DARB = (void *)SDL_GL_GetProcAddress("glCompressedTexSubImage2DARB");
		qglCompressedTexSubImage1DARB = (void *)SDL_GL_GetProcAddress("glCompressedTexSubImage1DARB");
		qglGetCompressedTexImageARB = (void *)SDL_GL_GetProcAddress("glGetCompressedTexImageARB");
		glRefConfig.arbTextureCompression = qtrue;
		ri.Printf(PRINT_ALL, result[glRefConfig.arbTextureCompression], extension);
	}

	// GL_EXT_framebuffer_multisample
	extension = "GL_EXT_framebuffer_multisample";
	glRefConfig.framebufferMultisample = qfalse;
	if (GLimp_HaveExtension(extension))
	{
		qglRenderbufferStorageMultisampleEXT = (void *)SDL_GL_GetProcAddress("glRenderbufferStorageMultisampleEXT");
		glRefConfig.framebufferMultisample = qtrue;
		ri.Printf(PRINT_ALL, result[glRefConfig.framebufferMultisample], extension);
	}
	else
	{
		ri.Printf(PRINT_ALL, result[2], extension);
	}

	glRefConfig.textureCompression = TCR_NONE;

	// GL_ARB_texture_compression_rgtc
	extension = "GL_ARB_texture_compression_rgtc";
	if (GLimp_HaveExtension(extension))
	{
		if (r_ext_compressed_textures->integer && glRefConfig.arbTextureCompression)
			glRefConfig.textureCompression |= TCR_RGTC;

		ri.Printf(PRINT_ALL, result[r_ext_compressed_textures->integer ? 1 : 0], extension);
	}
	else
	{
		ri.Printf(PRINT_ALL, result[2], extension);
	}

	glRefConfig.swizzleNormalmap = r_ext_compressed_textures->integer && !(glRefConfig.textureCompression & TCR_RGTC);

	// GL_ARB_texture_compression_bptc
	extension = "GL_ARB_texture_compression_bptc";
	if (GLimp_HaveExtension(extension))
	{
		if (r_ext_compressed_textures->integer >= 2)
			glRefConfig.textureCompression |= TCR_BPTC;

		ri.Printf(PRINT_ALL, result[(r_ext_compressed_textures->integer >= 2) ? 1 : 0], extension);
	}
	else
	{
		ri.Printf(PRINT_ALL, result[2], extension);
	}

	// GL_ARB_draw_buffers
	extension = "GL_ARB_draw_buffers";
	qglDrawBuffersARB = NULL;
	if( GLimp_HaveExtension( extension ) )
	{
		qglDrawBuffersARB = (void *) SDL_GL_GetProcAddress("glDrawBuffersARB");

		ri.Printf(PRINT_ALL, result[1], extension);
	}
	else
	{
		ri.Printf(PRINT_ALL, result[2], extension);
	}

	// GL_ARB_depth_clamp
	extension = "GL_ARB_depth_clamp";
	glRefConfig.depthClamp = qfalse;
	if( GLimp_HaveExtension( extension ) )
	{
		glRefConfig.depthClamp = qtrue;
		ri.Printf(PRINT_ALL, result[1], extension);
	}
	else
	{
		ri.Printf(PRINT_ALL, result[2], extension);
	}

	// GL_ARB_seamless_cube_map
	extension = "GL_ARB_seamless_cube_map";
	glRefConfig.seamlessCubeMap = qfalse;
	if( GLimp_HaveExtension( extension ) )
	{
		if (r_arb_seamless_cube_map->integer)
			glRefConfig.seamlessCubeMap = qtrue;

		ri.Printf(PRINT_ALL, result[glRefConfig.seamlessCubeMap], extension);
	}
	else
	{
		ri.Printf(PRINT_ALL, result[2], extension);
	}

	// GL_ARB_vertex_type_2_10_10_10_rev
	extension = "GL_ARB_vertex_type_2_10_10_10_rev";
	glRefConfig.packedNormalDataType = GL_BYTE;
	if( GLimp_HaveExtension( extension ) )
	{
		if (r_arb_vertex_type_2_10_10_10_rev->integer)
			glRefConfig.packedNormalDataType = GL_INT_2_10_10_10_REV;

		ri.Printf(PRINT_ALL, result[r_arb_vertex_type_2_10_10_10_rev->integer ? 1 : 0], extension);
	}
	else
	{
		ri.Printf(PRINT_ALL, result[2], extension);
	}

	// use float lightmaps?
	glRefConfig.floatLightmap = (glRefConfig.textureFloat && glRefConfig.halfFloatPixel && r_floatLightmap->integer && r_hdr->integer);

	// GL_ARB_vertex_array_object
	extension = "GL_ARB_vertex_array_object";
	glRefConfig.vertexArrayObject = qfalse;
	if( GLimp_HaveExtension( extension ) )
	{
		qglBindVertexArrayARB = (void *) SDL_GL_GetProcAddress("glBindVertexArray");
		qglDeleteVertexArraysARB = (void *) SDL_GL_GetProcAddress("glDeleteVertexArrays");
		qglGenVertexArraysARB = (void *) SDL_GL_GetProcAddress("glGenVertexArrays");
		qglIsVertexArrayARB = (void *) SDL_GL_GetProcAddress("glIsVertexArray");

		if (r_arb_vertex_array_object->integer)
			glRefConfig.vertexArrayObject = qtrue;

		ri.Printf(PRINT_ALL, result[glRefConfig.vertexArrayObject ? 1 : 0], extension);
	}
	else
	{
		ri.Printf(PRINT_ALL, result[2], extension);
	}

	// GL_ARB_half_float_vertex
	extension = "GL_ARB_half_float_vertex";
	glRefConfig.packedTexcoordDataType = GL_FLOAT;
	glRefConfig.packedTexcoordDataSize = sizeof(float) * 2;
	glRefConfig.packedColorDataType    = GL_FLOAT;
	glRefConfig.packedColorDataSize    = sizeof(float) * 4;
	if( GLimp_HaveExtension( extension ) )
	{
		if (r_arb_half_float_vertex->integer)
		{
			glRefConfig.packedTexcoordDataType = GL_HALF_FLOAT;
			glRefConfig.packedTexcoordDataSize = sizeof(uint16_t) * 2;
			glRefConfig.packedColorDataType    = GL_HALF_FLOAT;
			glRefConfig.packedColorDataSize    = sizeof(uint16_t) * 4;
		}

		ri.Printf(PRINT_ALL, result[r_arb_half_float_vertex->integer ? 1 : 0], extension);
	}
	else
	{
		ri.Printf(PRINT_ALL, result[2], extension);
	}

	// GL_EXT_direct_state_access
	extension = "GL_EXT_direct_state_access";

	qglBindMultiTexture = GLDSA_BindMultiTexture;
	qglTextureParameterf = GLDSA_TextureParameterf;
	qglTextureParameteri = GLDSA_TextureParameteri;
	qglTextureImage2D = GLDSA_TextureImage2D;
	qglTextureSubImage2D = GLDSA_TextureSubImage2D;
	qglCopyTextureImage2D = GLDSA_CopyTextureImage2D;
	qglCompressedTextureImage2D = GLDSA_CompressedTextureImage2D;
	qglCompressedTextureSubImage2D = GLDSA_CompressedTextureSubImage2D;
	qglGenerateTextureMipmap = GLDSA_GenerateTextureMipmap;

	qglProgramUniform1i = GLDSA_ProgramUniform1i;
	qglProgramUniform1f = GLDSA_ProgramUniform1f;
	qglProgramUniform2f = GLDSA_ProgramUniform2f;
	qglProgramUniform3f = GLDSA_ProgramUniform3f;
	qglProgramUniform4f = GLDSA_ProgramUniform4f;
	qglProgramUniform1fv = GLDSA_ProgramUniform1fv;
	qglProgramUniformMatrix4fv = GLDSA_ProgramUniformMatrix4fv;

	qglNamedRenderbufferStorage = GLDSA_NamedRenderbufferStorage;
	qglNamedRenderbufferStorageMultisample = GLDSA_NamedRenderbufferStorageMultisample;
	qglCheckNamedFramebufferStatus = GLDSA_CheckNamedFramebufferStatus;
	qglNamedFramebufferTexture2D = GLDSA_NamedFramebufferTexture2D;
	qglNamedFramebufferRenderbuffer = GLDSA_NamedFramebufferRenderbuffer;

	glRefConfig.directStateAccess = qfalse;
	if (GLimp_HaveExtension(extension))
	{
		if (r_ext_direct_state_access->integer)
		{
			glRefConfig.directStateAccess = qtrue;
			qglBindMultiTexture = (void *)SDL_GL_GetProcAddress("glBindMultiTextureEXT");
			qglTextureParameterf = (void *)SDL_GL_GetProcAddress("glTextureParameterfEXT");
			qglTextureParameteri = (void *)SDL_GL_GetProcAddress("glTextureParameteriEXT");
			qglTextureImage2D = (void *)SDL_GL_GetProcAddress("glTextureImage2DEXT");
			qglTextureSubImage2D = (void *)SDL_GL_GetProcAddress("glTextureSubImage2DEXT");
			qglCopyTextureImage2D = (void *)SDL_GL_GetProcAddress("glCopyTextureImage2DEXT");
			qglCompressedTextureImage2D = (void *)SDL_GL_GetProcAddress("glCompressedTextureImage2DEXT");
			qglCompressedTextureSubImage2D = (void *)SDL_GL_GetProcAddress("glCompressedTextureSubImage2DEXT");
			qglGenerateTextureMipmap = (void *)SDL_GL_GetProcAddress("glGenerateTextureMipmapEXT");

			qglProgramUniform1i = (void *)SDL_GL_GetProcAddress("glProgramUniform1iEXT");
			qglProgramUniform1f = (void *)SDL_GL_GetProcAddress("glProgramUniform1fEXT");
			qglProgramUniform2f = (void *)SDL_GL_GetProcAddress("glProgramUniform2fEXT");
			qglProgramUniform3f = (void *)SDL_GL_GetProcAddress("glProgramUniform3fEXT");
			qglProgramUniform4f = (void *)SDL_GL_GetProcAddress("glProgramUniform4fEXT");
			qglProgramUniform1fv = (void *)SDL_GL_GetProcAddress("glProgramUniform1fvEXT");
			qglProgramUniformMatrix4fv = (void *)SDL_GL_GetProcAddress("glProgramUniformMatrix4fvEXT");

			qglNamedRenderbufferStorage = (void *)SDL_GL_GetProcAddress("glNamedRenderbufferStorageEXT");
			qglNamedRenderbufferStorageMultisample = (void *)SDL_GL_GetProcAddress("glNamedRenderbufferStorageMultisampleEXT");
			qglCheckNamedFramebufferStatus = (void *)SDL_GL_GetProcAddress("glCheckNamedFramebufferStatusEXT");
			qglNamedFramebufferTexture2D = (void *)SDL_GL_GetProcAddress("glNamedFramebufferTexture2DEXT");
			qglNamedFramebufferRenderbuffer = (void *)SDL_GL_GetProcAddress("glNamedFramebufferRenderbufferEXT");
		}

		ri.Printf(PRINT_ALL, result[glRefConfig.directStateAccess ? 1 : 0], extension);
	}
	else
	{
		ri.Printf(PRINT_ALL, result[2], extension);
	}
}
