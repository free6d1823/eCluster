#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>


#include <malloc.h>
#include "datatype.h"

bool checkGlError(const char* funcName) {
    GLint err = glGetError();
    if (err != GL_NO_ERROR) {
        fprintf(stderr, "GL error after %s(): 0x%08x\n", funcName, err);
        return true;
    }
    return false;
}


GLuint createShader(GLenum shaderType, const char* src) {
    GLuint shader = glCreateShader(shaderType);
    if (!shader) {
        checkGlError("glCreateShader");
        return 0;
    }
    glShaderSource(shader, 1, &src, NULL);

    GLint compiled = GL_FALSE;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLogLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog) {
                glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);
                fprintf(stderr, "Could not compile %s shader:\n%s\n",
                        shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment",
                        infoLog);
                free(infoLog);
            }
        }
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}
GLuint CreateProgram(const char* vtxSrc, const char* fragSrc) {
    GLuint vtxShader = 0;
    GLuint fragShader = 0;
    GLuint program = 0;
    GLint linked = GL_FALSE;

    vtxShader = createShader(GL_VERTEX_SHADER, vtxSrc);
    if (!vtxShader)
        goto exit;

    fragShader = createShader(GL_FRAGMENT_SHADER, fragSrc);
    if (!fragShader)
        goto exit;

    program = glCreateProgram();
    if (!program) {
        checkGlError("glCreateProgram");
        goto exit;
    }
    glAttachShader(program, vtxShader);
    glAttachShader(program, fragShader);

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        fprintf(stderr, "Could not link program");
        GLint infoLogLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen) {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog) {
                glGetProgramInfoLog(program, infoLogLen, NULL, infoLog);
                fprintf(stderr, "Could not link program:\n%s\n", infoLog);
                free(infoLog);
            }
        }
        glDeleteProgram(program);
        program = 0;
    }

exit:
    glDeleteShader(vtxShader);
    glDeleteShader(fragShader);
    return program;
}

/////////////////////////////////////////////////////////////////////////////////
/****************************************************************/
nfImage* nfImage::create(unsigned int w, unsigned int h, unsigned int bpp)
{
    nfImage* img =  new nfImage;
    if (!img)
        return img;

    img->width = w;
    img->stride = w*bpp;
    img->height = h;
    img->bRef = false;
    img->buffer = (unsigned char*)malloc( img->stride*h);
    if (!img->buffer ) {
        delete img;
        return NULL;
    }

//    printf ("nfInitImage#%d (%dx%d)\n", img->seq, w, h);
    return img;
}
nfImage* nfImage::ref(unsigned char* data, unsigned int w, unsigned int h, unsigned int stride)
{
    nfImage* img =  new nfImage;
    if (!img)
        return img;

    img->buffer = data;
    img->width = w;
    img->stride = stride;
    img->height = h;
    img->bRef = true;
//    printf ("nfRefImage#%d (%dx%d)\n", img->seq, w, h);
    return img;
}
nfImage* nfImage::clone(nfImage* pSource)
{
    nfImage* img =  new nfImage;
    if (!img)
        return img;
    img->buffer = pSource->buffer;
    img->width = pSource->width;
    img->stride = pSource->stride;
    img->height = pSource->height;
    return img;
}

void nfImage::destroy(nfImage** ppImage)
{
    if(*ppImage) {
        if(!(*ppImage)->bRef)
            SAFE_FREE((*ppImage)->buffer);
        delete (*ppImage);
        *ppImage = NULL;
    }
}
nfPByte nfImage::dettach(nfImage** ppImage)
{
    nfPByte pBuffer = NULL;
    if(*ppImage) {
        pBuffer = (*ppImage)->buffer;
        delete (*ppImage);
        *ppImage = NULL;
    }
    return pBuffer;
}
/****************************************************************/
nfBuffer::nfBuffer(unsigned int /*elements*/)
{
}
nfBuffer* nfBuffer::create(unsigned int counts)
{
    return new nfBuffer(counts);
}

void nfBuffer::destroy(nfBuffer** /*ppBuffer*/)
{
}
nfBuffer::~nfBuffer()
{
}
/****************************************************************/
nfFloatBuffer::nfFloatBuffer(unsigned int elements)
{
    mBpu = sizeof(float);
    mCounts = elements;
    mLength = mBpu* elements;
    mpData = (float*) malloc(mLength);
//    LOGI("nfFloatBuffer new %dx%d=%d mpData=%p", mBpu, elements, mLength, mpData);
}
nfFloatBuffer::~nfFloatBuffer()
{
    if (mpData)
        free (mpData);
}
nfFloatBuffer* nfFloatBuffer::create(unsigned int elements)
{ 
    return new nfFloatBuffer(elements);
}
void nfFloatBuffer::destroy(nfFloatBuffer** ppBuffer)
{
    if(*ppBuffer) {
        delete (*ppBuffer);
        *ppBuffer = NULL;
    }
}
/****************************************************************/
nfUshortBuffer::nfUshortBuffer(unsigned int elements)
{
    mBpu = sizeof(unsigned int);
    mCounts = elements;
    mLength = mBpu* elements;
    mpData = (unsigned short*) malloc(mLength);
//    LOGI("nfUshortBuffer new %dx%d=%d mpData=%p", mBpu, elements, mLength, mpData);
}
nfUshortBuffer::~nfUshortBuffer()
{
    if (mpData)
        free (mpData);
}
nfUshortBuffer* nfUshortBuffer::create(unsigned int elements)
{  
    return new nfUshortBuffer(elements);
}
void nfUshortBuffer::destroy(nfUshortBuffer** ppBuffer)
{
    if(*ppBuffer) {
        delete (*ppBuffer);
        *ppBuffer = NULL;
    }
}


