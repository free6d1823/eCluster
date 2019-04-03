#include <vector>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

// Include GLEW
#include <GL/glew.h>

#define VERT_BUF 0
#define TEXT_BUF 1
#define TOTAL_BUF   2


#include "lib/datatype.h"
#include "lib/GlHelper.h"
#include "cubic.h"

static const char VERTEX_SHADER[] =
        "#version 330 core\n"
        "layout(location = 0) in vec3 vertexPosition;\n"
        "layout(location = 1) in  vec3 vertexColor;\n"
        "uniform mat4 MVP;\n"
        "out vec3 fragmentColor;\n"
        "void main() {\n"
                "  gl_Position = MVP*vec4(vertexPosition, 1);\n"
                "  fragmentColor = vertexColor;\n"
         "}\n";

static const char FRAGMENT_SHADER[] =
        "#version 330 core\n"
        "in vec3 fragmentColor;\n"
        "out vec3 color;\n"
         "void main() {\n"
                "color = fragmentColor;\n"
         "}\n";


static const nfFloat3D gpVertexBuf[] = {
    {-1.0f,-1.0f,-1.0f},
    {-1.0f,-1.0f, 1.0f},
    {-1.0f, 1.0f, 1.0f},
    { 1.0f, 1.0f,-1.0f},
    {-1.0f,-1.0f,-1.0f},
    {-1.0f, 1.0f,-1.0f},
    { 1.0f,-1.0f, 1.0f},
    {-1.0f,-1.0f,-1.0f},
    { 1.0f,-1.0f,-1.0f},
    { 1.0f, 1.0f,-1.0f},
    { 1.0f,-1.0f,-1.0f},
    {-1.0f,-1.0f,-1.0f},
    {-1.0f,-1.0f,-1.0f},
    {-1.0f, 1.0f, 1.0f},
    {-1.0f, 1.0f,-1.0f},
    { 1.0f,-1.0f, 1.0f},
    {-1.0f,-1.0f, 1.0f},
    {-1.0f,-1.0f,-1.0f},
    {-1.0f, 1.0f, 1.0f},
    {-1.0f,-1.0f, 1.0f},
    { 1.0f,-1.0f, 1.0f},
    { 1.0f, 1.0f, 1.0f},
    { 1.0f,-1.0f,-1.0f},
    { 1.0f, 1.0f,-1.0f},
    { 1.0f,-1.0f,-1.0f},
    { 1.0f, 1.0f, 1.0f},
    { 1.0f,-1.0f, 1.0f},
    { 1.0f, 1.0f, 1.0f},
    { 1.0f, 1.0f,-1.0f},
    {-1.0f, 1.0f,-1.0f},
    { 1.0f, 1.0f, 1.0f},
    {-1.0f, 1.0f,-1.0f},
    {-1.0f, 1.0f, 1.0f},
    { 1.0f, 1.0f, 1.0f},
    {-1.0f, 1.0f, 1.0f},
    { 1.0f,-1.0f, 1.0f}
};
static const nfFloat3D gpColorBuf[] = {
    {0.583f,  0.771f,  0.014f},
    {0.609f,  0.115f,  0.436f},
    {0.327f,  0.483f,  0.844f},
    {0.822f,  0.569f,  0.201f},
    {0.435f,  0.602f,  0.223f},
    { 0.310f,  0.747f,  0.185f},
    { 0.597f,  0.770f,  0.761f},
    { 0.559f,  0.436f,  0.730f},
    { 0.359f,  0.583f,  0.152f},
    { 0.483f,  0.596f,  0.789f},
    { 0.559f,  0.861f,  0.639f},
    { 0.195f,  0.548f,  0.859f},
    { 0.014f,  0.184f,  0.576f},
    { 0.771f,  0.328f,  0.970f},
    { 0.406f,  0.615f,  0.116f},
    { 0.676f,  0.977f,  0.133f},
    { 0.971f,  0.572f,  0.833f},
    { 0.140f,  0.616f,  0.489f},
    { 0.997f,  0.513f,  0.064f},
    { 0.945f,  0.719f,  0.592f},
    { 0.543f,  0.021f,  0.978f},
    { 0.279f,  0.317f,  0.505f},
    { 0.167f,  0.620f,  0.077f},
    { 0.347f,  0.857f,  0.137f},
    { 0.055f,  0.953f,  0.042f},
    { 0.714f,  0.505f,  0.345f},
    { 0.783f,  0.290f,  0.734f},
    { 0.722f,  0.645f,  0.174f},
    { 0.302f,  0.455f,  0.848f},
    { 0.225f,  0.587f,  0.040f},
    { 0.517f,  0.713f,  0.338f},
    { 0.053f,  0.959f,  0.120f},
    { 0.393f,  0.621f,  0.362f},
    { 0.673f,  0.211f,  0.457f},
    { 0.820f,  0.883f,  0.371f},
    { 0.982f,  0.099f,  0.879f}
};


Cubic::Cubic():mProgramId(0),
        mMvpMatrixUniform(0), mVaoId(0)
{
    mNumToDraw = 0;
    mVertexBufId[0] = mVertexBufId[1] = 0;



}
Cubic::~Cubic()
{
}

bool Cubic::init()
{
    glGenVertexArrays(1, &mVaoId);
    glBindVertexArray(mVaoId);

    mProgramId = CreateProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    if (!mProgramId)
        return false;

    mMvpMatrixUniform = glGetUniformLocation(mProgramId, "MVP");
    glGenBuffers(2, mVertexBufId);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufId[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gpVertexBuf), &gpVertexBuf[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufId[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gpColorBuf), &gpColorBuf[0], GL_STATIC_DRAW);

    mNumToDraw = sizeof(gpVertexBuf)/sizeof(gpVertexBuf[0]);

    printf("mNumToDraw = %d\n", mNumToDraw);

    mMatModel = Mat4::Identity();

}

void Cubic::transform(Mat4& transform)
{
    mMatModel = mMatModel * transform;
}
void Cubic::update(Mat4& pojection, Mat4& view, Vec3& light)
{
    Mat4 MVP = pojection * view * mMatModel;
    glUseProgram(mProgramId);

    //glUniform3f(m_lightID, light.x, light.y, light.z);
    glUniformMatrix4fv(mMvpMatrixUniform, 1, GL_FALSE, MVP.Ptr());



}

void Cubic::cleanup()
{


    if (mVaoId) {
        glDeleteVertexArrays(1, &mVaoId);
        mVaoId = 0;
    }
   if (mVertexBufId[0]) {
        glDeleteBuffers(2, mVertexBufId);;
        mVertexBufId[0] = 0;
    }

    if(mProgramId) {
        glDeleteProgram(mProgramId);
        mProgramId = 0;
    }

}

void Cubic::draw(bool bReload)
{
    if (bReload) {
        //update texture image
    }
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufId[0]);
    glVertexAttribPointer(
        0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufId[1]);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    glDrawArrays(GL_TRIANGLES, 0, mNumToDraw);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

