/* WARNING: This file was automatically generated */
/* Do not edit. */

#include "flextGL.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(WIN32)
#define FLEXT_CALL __stdcall
#else
#define FLEXT_CALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* FLEXT_C_EXTENSION
 * Compiling in strict c leads to errors due to void* to function ptr
 * cast being illegal. Mark as extension so that the compiler will
 * accept it.
 */
#if defined(__GNUC__) || defined(__clang__)
#define FLEXT_C_EXTENSION(x) __extension__(x)
#else
#define FLEXT_C_EXTENSION(x) x
#endif

typedef void(FLEXT_CALL *GLPROC)();

void flextLoadOpenGLFunctions(void);

static void open_libgl(void);
static void close_libgl(void);
static GLPROC get_proc(const char *proc);
static void add_extension(const char* extension);

int flextInit(void)
{
    GLint minor, major;
    GLint num_extensions;
    int i;

    open_libgl();
    flextLoadOpenGLFunctions();
    close_libgl();

    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    /* --- Check for minimal version and profile --- */

    if (major * 10 + minor < 33) {
#if !defined(FLEXT_NO_LOGGING)
        fprintf(stderr, "Error: OpenGL version 3.3 not supported.\n");
        fprintf(stderr, "       Your version is %d.%d.\n", major, minor);
        fprintf(stderr, "       Try updating your graphics driver.\n");
#endif
        return GL_FALSE;
    }


    /* --- Check for extensions --- */

    glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);

    for (i = 0; i < num_extensions; i++) {
        add_extension((const char*)glGetStringi(GL_EXTENSIONS, i));
    }


    return GL_TRUE;
}



void flextLoadOpenGLFunctions(void)
{
    /* --- Function pointer loading --- */

    int i;
    for(i = 0 ; i < 283 ; i++){
        glpf.fp[i] = get_proc(glpf.fp[i]);
    }
}

/* ----------------------- Extension flag definitions ---------------------- */

/* ---------------------- Function pointer definitions --------------------- */
GLPF glpf = { {
/* GL_VERSION_1_2 */
/* 4 functions */
"glCopyTexSubImage3D",
"glDrawRangeElements",
"glTexImage3D",
"glTexSubImage3D",

/* GL_VERSION_1_3 */
/* 9 functions */
"glActiveTexture",
"glCompressedTexImage1D",
"glCompressedTexImage2D",
"glCompressedTexImage3D",
"glCompressedTexSubImage1D",
"glCompressedTexSubImage2D",
"glCompressedTexSubImage3D",
"glGetCompressedTexImage",
"glSampleCoverage",

/* GL_VERSION_1_4 */
/* 9 functions */
"glBlendColor",
"glBlendEquation",
"glBlendFuncSeparate",
"glMultiDrawArrays",
"glMultiDrawElements",
"glPointParameterf",
"glPointParameterfv",
"glPointParameteri",
"glPointParameteriv",

/* GL_VERSION_1_5 */
/* 19 functions */
"glBeginQuery",
"glBindBuffer",
"glBufferData",
"glBufferSubData",
"glDeleteBuffers",
"glDeleteQueries",
"glEndQuery",
"glGenBuffers",
"glGenQueries",
"glGetBufferParameteriv",
"glGetBufferPointerv",
"glGetBufferSubData",
"glGetQueryObjectiv",
"glGetQueryObjectuiv",
"glGetQueryiv",
"glIsBuffer",
"glIsQuery",
"glMapBuffer",
"glUnmapBuffer",

/* GL_VERSION_2_0 */
/* 93 functions */
"glAttachShader",
"glBindAttribLocation",
"glBlendEquationSeparate",
"glCompileShader",
"glCreateProgram",
"glCreateShader",
"glDeleteProgram",
"glDeleteShader",
"glDetachShader",
"glDisableVertexAttribArray",
"glDrawBuffers",
"glEnableVertexAttribArray",
"glGetActiveAttrib",
"glGetActiveUniform",
"glGetAttachedShaders",
"glGetAttribLocation",
"glGetProgramInfoLog",
"glGetProgramiv",
"glGetShaderInfoLog",
"glGetShaderSource",
"glGetShaderiv",
"glGetUniformLocation",
"glGetUniformfv",
"glGetUniformiv",
"glGetVertexAttribPointerv",
"glGetVertexAttribdv",
"glGetVertexAttribfv",
"glGetVertexAttribiv",
"glIsProgram",
"glIsShader",
"glLinkProgram",
"glShaderSource",
"glStencilFuncSeparate",
"glStencilMaskSeparate",
"glStencilOpSeparate",
"glUniform1f",
"glUniform1fv",
"glUniform1i",
"glUniform1iv",
"glUniform2f",
"glUniform2fv",
"glUniform2i",
"glUniform2iv",
"glUniform3f",
"glUniform3fv",
"glUniform3i",
"glUniform3iv",
"glUniform4f",
"glUniform4fv",
"glUniform4i",
"glUniform4iv",
"glUniformMatrix2fv",
"glUniformMatrix3fv",
"glUniformMatrix4fv",
"glUseProgram",
"glValidateProgram",
"glVertexAttrib1d",
"glVertexAttrib1dv",
"glVertexAttrib1f",
"glVertexAttrib1fv",
"glVertexAttrib1s",
"glVertexAttrib1sv",
"glVertexAttrib2d",
"glVertexAttrib2dv",
"glVertexAttrib2f",
"glVertexAttrib2fv",
"glVertexAttrib2s",
"glVertexAttrib2sv",
"glVertexAttrib3d",
"glVertexAttrib3dv",
"glVertexAttrib3f",
"glVertexAttrib3fv",
"glVertexAttrib3s",
"glVertexAttrib3sv",
"glVertexAttrib4Nbv",
"glVertexAttrib4Niv",
"glVertexAttrib4Nsv",
"glVertexAttrib4Nub",
"glVertexAttrib4Nubv",
"glVertexAttrib4Nuiv",
"glVertexAttrib4Nusv",
"glVertexAttrib4bv",
"glVertexAttrib4d",
"glVertexAttrib4dv",
"glVertexAttrib4f",
"glVertexAttrib4fv",
"glVertexAttrib4iv",
"glVertexAttrib4s",
"glVertexAttrib4sv",
"glVertexAttrib4ubv",
"glVertexAttrib4uiv",
"glVertexAttrib4usv",
"glVertexAttribPointer",

/* GL_VERSION_2_1 */
/* 6 functions */
"glUniformMatrix2x3fv",
"glUniformMatrix2x4fv",
"glUniformMatrix3x2fv",
"glUniformMatrix3x4fv",
"glUniformMatrix4x2fv",
"glUniformMatrix4x3fv",

/* GL_VERSION_3_0 */
/* 84 functions */
"glBeginConditionalRender",
"glBeginTransformFeedback",
"glBindBufferBase",
"glBindBufferRange",
"glBindFragDataLocation",
"glBindFramebuffer",
"glBindRenderbuffer",
"glBindVertexArray",
"glBlitFramebuffer",
"glCheckFramebufferStatus",
"glClampColor",
"glClearBufferfi",
"glClearBufferfv",
"glClearBufferiv",
"glClearBufferuiv",
"glColorMaski",
"glDeleteFramebuffers",
"glDeleteRenderbuffers",
"glDeleteVertexArrays",
"glDisablei",
"glEnablei",
"glEndConditionalRender",
"glEndTransformFeedback",
"glFlushMappedBufferRange",
"glFramebufferRenderbuffer",
"glFramebufferTexture1D",
"glFramebufferTexture2D",
"glFramebufferTexture3D",
"glFramebufferTextureLayer",
"glGenFramebuffers",
"glGenRenderbuffers",
"glGenVertexArrays",
"glGenerateMipmap",
"glGetBooleani_v",
"glGetFragDataLocation",
"glGetFramebufferAttachmentParameteriv",
"glGetIntegeri_v",
"glGetRenderbufferParameteriv",
"glGetStringi",
"glGetTexParameterIiv",
"glGetTexParameterIuiv",
"glGetTransformFeedbackVarying",
"glGetUniformuiv",
"glGetVertexAttribIiv",
"glGetVertexAttribIuiv",
"glIsEnabledi",
"glIsFramebuffer",
"glIsRenderbuffer",
"glIsVertexArray",
"glMapBufferRange",
"glRenderbufferStorage",
"glRenderbufferStorageMultisample",
"glTexParameterIiv",
"glTexParameterIuiv",
"glTransformFeedbackVaryings",
"glUniform1ui",
"glUniform1uiv",
"glUniform2ui",
"glUniform2uiv",
"glUniform3ui",
"glUniform3uiv",
"glUniform4ui",
"glUniform4uiv",
"glVertexAttribI1i",
"glVertexAttribI1iv",
"glVertexAttribI1ui",
"glVertexAttribI1uiv",
"glVertexAttribI2i",
"glVertexAttribI2iv",
"glVertexAttribI2ui",
"glVertexAttribI2uiv",
"glVertexAttribI3i",
"glVertexAttribI3iv",
"glVertexAttribI3ui",
"glVertexAttribI3uiv",
"glVertexAttribI4bv",
"glVertexAttribI4i",
"glVertexAttribI4iv",
"glVertexAttribI4sv",
"glVertexAttribI4ubv",
"glVertexAttribI4ui",
"glVertexAttribI4uiv",
"glVertexAttribI4usv",
"glVertexAttribIPointer",

/* GL_VERSION_3_1 */
/* 12 functions */
"glCopyBufferSubData",
"glDrawArraysInstanced",
"glDrawElementsInstanced",
"glGetActiveUniformBlockName",
"glGetActiveUniformBlockiv",
"glGetActiveUniformName",
"glGetActiveUniformsiv",
"glGetUniformBlockIndex",
"glGetUniformIndices",
"glPrimitiveRestartIndex",
"glTexBuffer",
"glUniformBlockBinding",

/* GL_VERSION_3_2 */
/* 19 functions */
"glClientWaitSync",
"glDeleteSync",
"glDrawElementsBaseVertex",
"glDrawElementsInstancedBaseVertex",
"glDrawRangeElementsBaseVertex",
"glFenceSync",
"glFramebufferTexture",
"glGetBufferParameteri64v",
"glGetInteger64i_v",
"glGetInteger64v",
"glGetMultisamplefv",
"glGetSynciv",
"glIsSync",
"glMultiDrawElementsBaseVertex",
"glProvokingVertex",
"glSampleMaski",
"glTexImage2DMultisample",
"glTexImage3DMultisample",
"glWaitSync",

/* GL_VERSION_3_3 */
/* 28 functions */
"glBindFragDataLocationIndexed",
"glBindSampler",
"glDeleteSamplers",
"glGenSamplers",
"glGetFragDataIndex",
"glGetQueryObjecti64v",
"glGetQueryObjectui64v",
"glGetSamplerParameterIiv",
"glGetSamplerParameterIuiv",
"glGetSamplerParameterfv",
"glGetSamplerParameteriv",
"glIsSampler",
"glQueryCounter",
"glSamplerParameterIiv",
"glSamplerParameterIuiv",
"glSamplerParameterf",
"glSamplerParameterfv",
"glSamplerParameteri",
"glSamplerParameteriv",
"glVertexAttribDivisor",
"glVertexAttribP1ui",
"glVertexAttribP1uiv",
"glVertexAttribP2ui",
"glVertexAttribP2uiv",
"glVertexAttribP3ui",
"glVertexAttribP3uiv",
"glVertexAttribP4ui",
"glVertexAttribP4uiv",

} };


static void add_extension(const char* extension)
{
}


/* ------------------ get_proc from Slavomir Kaslev's gl3w ----------------- */

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

static HMODULE libgl;

static void open_libgl(void)
{
    libgl = LoadLibraryA("opengl32.dll");
}

static void close_libgl(void)
{
    FreeLibrary(libgl);
}

static GLPROC get_proc(const char *proc)
{
    GLPROC res;

    res = wglGetProcAddress(proc);
    if (!res)
        res = GetProcAddress(libgl, proc);
    return res;
}
#elif defined(__APPLE__) || defined(__APPLE_CC__)
#include <Carbon/Carbon.h>

CFBundleRef bundle;
CFURLRef bundleURL;

static void open_libgl(void)
{
    bundleURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
                CFSTR("/System/Library/Frameworks/OpenGL.framework"),
                kCFURLPOSIXPathStyle, true);

    bundle = CFBundleCreate(kCFAllocatorDefault, bundleURL);
    assert(bundle != NULL);
}

static void close_libgl(void)
{
    CFRelease(bundle);
    CFRelease(bundleURL);
}

static GLPROC get_proc(const char *proc)
{
    GLPROC res;

    CFStringRef procname = CFStringCreateWithCString(kCFAllocatorDefault, proc,
                kCFStringEncodingASCII);
    FLEXT_C_EXTENSION(res = CFBundleGetFunctionPointerForName(bundle, procname));
    CFRelease(procname);
    return res;
}
#elif defined(ANDROID)
#include <EGL/egl.h>

static void open_libgl(void)
{
    // nothing to do
}

static void close_libgl(void)
{
    // nothing to do
}

static GLPROC get_proc(const char *proc)
{
    GLPROC res;
    res = eglGetProcAddress((const char *) proc);
    return res;
}
#else
#include <dlfcn.h>
#include <GL/glx.h>

static void *libgl;

static void open_libgl(void)
{
    libgl = dlopen("libGL.so.1", RTLD_LAZY | RTLD_GLOBAL);
}

static void close_libgl(void)
{
    dlclose(libgl);
}

static GLPROC get_proc(const char *proc)
{
    GLPROC res;

    res = glXGetProcAddress((const GLubyte *) proc);
    if (!res) {
        FLEXT_C_EXTENSION(res = dlsym(libgl, proc));
    }
    return res;
}
#endif

#ifdef __cplusplus
}
#endif
