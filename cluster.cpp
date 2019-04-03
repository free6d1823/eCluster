#include <vector>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

// Include GLEW
#include <GL/glew.h>

#define VERT_BUF 0
#define TEXT_BUF 1
#define TOTAL_BUF   2

#include "SOIL/SOIL.h"
#include "lib/datatype.h"
#include "lib/GlHelper.h"
#include "cluster.h"

#define MAX_NEEDLE_VALUE 10000
#define MIN_NEEDLE_VALUE 0
#define ANGLE_PER_UNIT   (124+125)*M_PI/(MAX_NEEDLE_VALUE*180)
#define BASE_NEEDLE_ANGLE   236*M_PI/180

#define MAX_LEVEL 100
#define MIN_LEVEL 0
#define ANGLE_PER_LEVEL   (-(24+24)*M_PI/((MAX_LEVEL-(MIN_LEVEL))*180))
#define BASE_LEVEL_ANGLE   24*M_PI/180

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

static const char VERTEX_SHADER_TEXTURE[] =
        "#version 330 core\n"
        "layout(location = 0) in vec3 vertexPosition;\n"
        "layout(location = 1) in  vec2 vertexUv;\n"
        "uniform mat4 MVP;\n"
        "out vec2 Uv;\n"
        "void main() {\n"
                "  gl_Position = MVP*vec4(vertexPosition, 1);\n"
                "  Uv = vertexUv;\n"
         "}\n";

static const char FRAGMENT_SHADER_TEXTURE[] =
        "#version 330 core\n"
        "in vec2 Uv;\n"
        "out vec3 color;\n"\
        "uniform sampler2D myTextureSampler;\n"
         "void main() {\n"
                "color = texture(myTextureSampler, Uv).rgb;\n"
         "}\n";

static const nfFloat3D gpNeedleVertBuf[] = {
        {0.000f, 0.904f, 0.010f},
        {0.000f, 0.876f, 0.010f},
        {0.000f, 0.608f, 0.010f},

        {0.025f, 0.876f, 0.005f},
        {0.025f, 0.608f, 0.005f},
        {-.025f, 0.876f, 0.005f},
        {-.025f, 0.608f, 0.005f},
    //bar
        {0.100f, 0.592f, 0.010f}, //7
        {0.100f, 0.572f, 0.010f}, //8
        {0.103f, 0.592f, 0.005f}, //9
        {0.103f, 0.572f, 0.005f}, //10

        {-.100f, 0.592f, 0.010f}, //11
        {-.100f, 0.572f, 0.010f}, //12
        {-0.103f, 0.592f, 0.05f},
        {-0.103f, 0.572f, 0.05f},

    };

static const nfFloat3D gpNeedleColorBuf[] = {
    {0.983f,  0.09f,  0.09f},
    {0.609f,  0.01f,  0.04f},
    {0.827f,  0.04f,  0.08f},
    {0.822f,  0.05f,  0.02f},
    {0.835f,  0.02f,  0.02f},
    { 0.410f,  0.07f,  0.01f},
    { 0.497f,  0.07f,  0.07f},
    { 0.759f,  0.1f,  0.07f},
    { 0.859f,  0.15f,  0.01f},
    {0.483f,  0.07f,  0.01f},
    {0.409f,  0.11f,  0.04f},
    { 0.759f,  0.1f,  0.07f},
    { 0.859f,  0.15f,  0.01f},
    {0.483f,  0.07f,  0.01f},
    {0.409f,  0.11f,  0.04f},
};

static const unsigned short gpNeedleIndexBuf[] = {
    0,1,3, 1,2,3, 3,2,4,
    0,5,1,1,5,2,2,5,6,
    7,11,8, 8,11,12,
    7,8,9,9,8,10,
    11,13,12,12,13,14
};

static const nfFloat3D gpPinVertBuf[] = {
        {0.000f, -0.800f, 0.010f},
        {0.000f, -0.924f, 0.010f},
        {0.005f, -0.800f, 0.008f},
        {0.005f, -0.924f, 0.008f},
        {0.046f, -0.796f, 0.004f},
        {0.046f, -0.880f, 0.004f},

        {-0.005f, -0.800f, 0.008f},
        {-0.005f, -0.924f, 0.008f},
        {-0.046f, -0.796f, 0.004f},
        {-0.046f, -0.880f, 0.004f}
};
static const nfFloat3D gpPinColorBuf[] = {
    {0.983f,  0.09f,  0.09f},
    {0.909f,  0.01f,  0.04f},
    {0.827f,  0.04f,  0.08f},
    {0.822f,  0.05f,  0.02f},
    {0.515f,  0.02f,  0.02f},
    { 0.510f,  0.07f,  0.01f},

    { 0.797f,  0.07f,  0.07f},
    { 0.759f,  0.1f,  0.07f},
    { 0.409f,  0.15f,  0.01f},
    { 0.403f,  0.07f,  0.01f},

};
static const unsigned short gpPinIndexBuf[] = {
    0,1,2, 2,1,3,2,3,5,2,5,4,
    0,6,1,6,7,1,6,8,9,7,6,9
};

Needle::Needle():mProgramId(0),
        mMvpMatrixUniform(0), mVaoId(0)
{
    mNumToDraw = 0;
    mVertexBufId[0] = mVertexBufId[1] = mVertexBufId[2] = -1;



}
Needle::~Needle()
{
}

bool Needle::init(float scale, Vec3 pos)
{
    glGenVertexArrays(1, &mVaoId);
    glBindVertexArray(mVaoId);

    mProgramId = CreateProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    if (!mProgramId)
        return false;

    mMvpMatrixUniform = glGetUniformLocation(mProgramId, "MVP");
    glGenBuffers(3, mVertexBufId);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufId[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gpNeedleVertBuf), &gpNeedleVertBuf[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufId[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gpNeedleColorBuf), &gpNeedleColorBuf[0], GL_STATIC_DRAW);

    mNumToDraw =sizeof(gpNeedleIndexBuf)/sizeof(unsigned short);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVertexBufId[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gpNeedleIndexBuf), &gpNeedleIndexBuf[0], GL_STATIC_DRAW);

    mMatModel = Mat4::Translation(pos.getX(),pos.getY(),pos.getZ())*Mat4::RotationZ(BASE_NEEDLE_ANGLE);
    mMatRotate = Mat4::Identity();
    mMatMvp = Mat4::Identity();
}

void Needle::transform(Mat4& transform)
{

}

void Needle::setRpm(int rpm)
{
    mMatRotate = Mat4::RotationZ(ANGLE_PER_UNIT*(float)rpm);
}

void Needle::update(Mat4& pojection, Mat4& view, Vec3& light)
{
    mMatMvp = pojection * view * mMatModel*mMatRotate ;




}

void Needle::cleanup()
{


    if (mVaoId) {
        glDeleteVertexArrays(1, &mVaoId);
        mVaoId = 0;
    }
    if (mVertexBufId[0]>= 0) {
         glDeleteBuffers(3, mVertexBufId);;
         mVertexBufId[0] = -1;
         mVertexBufId[1] = -1;
         mVertexBufId[2] = -1;
     }

    if(mProgramId) {
        glDeleteProgram(mProgramId);
        mProgramId = 0;
    }

}

void Needle::draw(bool bReload)
{
    glUseProgram(mProgramId);
    glUniformMatrix4fv(mMvpMatrixUniform, 1, GL_FALSE, mMatMvp.Ptr());

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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVertexBufId[2]);
    glDrawElements(GL_TRIANGLES, mNumToDraw, GL_UNSIGNED_SHORT, 0 );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
///////////////////////////////////////////////////////////////////////////////////////////////
Pin::Pin():mProgramId(0),
        mMvpMatrixUniform(0), mVaoId(0)
{
    mNumToDraw = 0;
    mVertexBufId[0] = mVertexBufId[1] = mVertexBufId[2] = -1;



}
Pin::~Pin()
{
}

bool Pin::init(float scale, Vec3 pos)
{
    glGenVertexArrays(1, &mVaoId);
    glBindVertexArray(mVaoId);

    mProgramId = CreateProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    if (!mProgramId)
        return false;

    mMvpMatrixUniform = glGetUniformLocation(mProgramId, "MVP");
    glGenBuffers(3, mVertexBufId);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufId[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gpPinVertBuf), &gpPinVertBuf[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufId[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gpPinColorBuf), &gpPinColorBuf[0], GL_STATIC_DRAW);

    mNumToDraw =sizeof(gpNeedleIndexBuf)/sizeof(unsigned short);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVertexBufId[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gpPinIndexBuf), &gpPinIndexBuf[0], GL_STATIC_DRAW);

    mMatModel = Mat4::Translation(pos.getX(),pos.getY(),pos.getZ())*Mat4::RotationZ(BASE_LEVEL_ANGLE);
    mMatRotate = Mat4::Identity();
    mMatMvp = Mat4::Identity();
}

void Pin::transform(Mat4& transform)
{

}

void Pin::setLevel(float level)
{
    mMatRotate = Mat4::RotationZ( ANGLE_PER_LEVEL* level);
}

void Pin::update(Mat4& pojection, Mat4& view, Vec3& light)
{
    mMatMvp = pojection * view * mMatModel*mMatRotate ;




}

void Pin::cleanup()
{


    if (mVaoId) {
        glDeleteVertexArrays(1, &mVaoId);
        mVaoId = 0;
    }
    if (mVertexBufId[0]>= 0) {
         glDeleteBuffers(3, mVertexBufId);;
         mVertexBufId[0] = -1;
         mVertexBufId[1] = -1;
         mVertexBufId[2] = -1;
     }

    if(mProgramId) {
        glDeleteProgram(mProgramId);
        mProgramId = 0;
    }

}

void Pin::draw(bool bReload)
{
    glUseProgram(mProgramId);
    glUniformMatrix4fv(mMvpMatrixUniform, 1, GL_FALSE, mMatMvp.Ptr());

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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVertexBufId[2]);
    glDrawElements(GL_TRIANGLES, mNumToDraw, GL_UNSIGNED_SHORT, 0 );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

///////////////////////////////


/// \brief Disc::Disc
///

Disc::Disc():mProgramId(0),
        mMvpMatrixUniform(0), mVaoId(0), mTextureData(-1), mTextureUniform(-1)
{
    mNumToDraw = 0;
    mVertexBufId[0] = mVertexBufId[1] =  mVertexBufId[2] = -1;



}
Disc::~Disc()
{
}
bool Disc::load(float r, float tx, vector < Vec3 > & vertices, vector <Vec2 > & uvs, vector < unsigned short > & index)
{
    int mNumDiv = 64;
    double dTheda = 2.0*M_PI/mNumDiv;
    double theda = 0.0;
    int i,j;
    float u0 = 0;
    float v0 = 0;
    float x0,y0,z0;
    z0 = 0.0f;
//    p0 <--p2 (phi)
//     |   ^
//     |  /
//     | /
//     V/
//    p1

    vertices.push_back(Vec3(tx,0,z0));
    uvs.push_back(Vec2(0.5f, 0.5f));

    for (i=1; i<= (int) mNumDiv; i++) {
        x0 = r* sin(theda) + tx;
        y0 = r* cos(theda);
        vertices.push_back(Vec3(x0,y0,z0));
        u0 = 0.5 + 0.5*sin(theda);
        v0 = 0.5 - 0.5*cos(theda);
        uvs.push_back(Vec2(u0, v0));

        theda += dTheda;

        index.push_back(0);
        if (i== mNumDiv)
            index.push_back(1);
        else
            index.push_back(i+1);
        index.push_back(i);

    }

}

bool Disc::init(float scale, Vec3 pos, const char* bkgnd)
{
    glGenVertexArrays(1, &mVaoId);
    glBindVertexArray(mVaoId);

    mProgramId = CreateProgram(VERTEX_SHADER_TEXTURE, FRAGMENT_SHADER_TEXTURE);
    if (!mProgramId)
        return false;

    mTextureData = SOIL_load_OGL_texture(bkgnd,
                      SOIL_LOAD_AUTO,
                      SOIL_CREATE_NEW_ID,
                      SOIL_FLAG_POWER_OF_TWO| SOIL_FLAG_MIPMAPS);

    mTextureUniform = glGetUniformLocation(mProgramId, "myTextureSampler");

    printf("mTextureData=%d  mTextureUniform = %d\n", mTextureData, mTextureUniform);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    vector<Vec3> vertices;
    vector<Vec2> uvs;
    vector <unsigned short> ind;
    bool res = load(scale, pos.getX(), vertices, uvs, ind);

    mMvpMatrixUniform = glGetUniformLocation(mProgramId, "MVP");
    glGenBuffers(3, mVertexBufId);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufId[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vec3), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufId[1]);
    glBufferData(GL_ARRAY_BUFFER, uvs.size()*sizeof(Vec2), &uvs[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVertexBufId[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size()* sizeof(unsigned short), &ind[0], GL_STATIC_DRAW);

    mNumToDraw = ind.size();

    printf("Disc mNumToDraw = %d\n", mNumToDraw);

    mMatModel = Mat4::Identity();

}

void Disc::transform(Mat4& transform)
{
    mMatModel = mMatModel * transform;
}
void Disc::update(Mat4& pojection, Mat4& view, Vec3& light)
{
    mMatMvp = pojection * view * mMatModel;



}

void Disc::cleanup()
{


    if (mVaoId) {
        glDeleteVertexArrays(1, &mVaoId);
        mVaoId = 0;
    }
   if (mVertexBufId[0]>= 0) {
        glDeleteBuffers(3, mVertexBufId);;
        mVertexBufId[0] = -1;
        mVertexBufId[1] = -1;
        mVertexBufId[2] = -1;
    }

    if(mProgramId) {
        glDeleteProgram(mProgramId);
        mProgramId = 0;
    }
    if(mTextureData)
        glDeleteTextures(1, &mTextureData);

}

void Disc::draw(bool bReload)
{
    glUseProgram(mProgramId);
    glUniformMatrix4fv(mMvpMatrixUniform, 1, GL_FALSE, mMatMvp.Ptr());
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureData);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(mTextureUniform, 0);


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
        2,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVertexBufId[2]);
    glDrawElements(GL_TRIANGLES, mNumToDraw, GL_UNSIGNED_SHORT, 0 );
    //glDrawArrays(GL_TRIANGLES, 0, mNumToDraw);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
