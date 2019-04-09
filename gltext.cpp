#include "gltext.h"
#include "SOIL/SOIL.h"
#include "lib/datatype.h"
#include "lib/GlHelper.h"

#include <ft2build.h>
#include FT_FREETYPE_H

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

GlText::GlText():mProgramId(0),
        mMvpMatrixUniform(0), mVaoId(0), mTextureData(-1), mTextureUniform(-1)
{
    mNumToDraw = 0;
    mVertexBufId[0] = mVertexBufId[1] =  mVertexBufId[2] = -1;

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        fprintf(stderr, "ERROR::FREETYPE: Could not init FreeType Library");

    FT_Face face;
    if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
         fprintf(stderr, "ERROR::FREETYPE: Failed to load font");
FT_Set_Pixel_Sizes(face, 0, 48);

}
GlText::~GlText()
{
}
bool GlText::load(float r, Vec3& pos, vector < Vec3 > & vertices, vector <Vec2 > & uvs, vector < unsigned short > & index)
{
    int i,j;
#define lats 32
#define longs 32
    const float dv = 1.0/lats;
    const float du = 1.0/longs;

    float u0 = 0;
    float v0 = 0;
    float x0,y0,z0;
    float x1,y1,z1;
    float x2,z2; //y2=y0
    float x3,z3; //y3=y1
    double theda, phi=M_PI/2;
    const double dtheda = 2*M_PI/longs;
    const double dphi = M_PI/lats;

    int k = 0;
    for(i = 1; i < lats; i++) {
        u0 = 0;
        v0 += dv;
        phi -= dphi;
        theda = 0;
        y0 = r*sin(phi) + pos.getY();
        y1 = r*sin(phi-dphi)+ pos.getY();
        x0 = pos.getX();
        z0 = r*cos(phi)+ pos.getZ();

        double phi2 = phi - dphi;
//    p0 <--p2 (phi)
//     |   ^/^
//     |  // |
//     | //  |
//     V/V   |
//    p1 -->p3 (phi2)

        for(j = 0; j <= longs; j++) {
            double theda2 = theda + dtheda;

            x1 = r*(float) (cos(phi2)*sin(theda)) + pos.getX();
            x2 = r*(float) (cos(phi)*sin(theda2)) + pos.getX();
            x3 = r*(float) (cos(phi2)*sin(theda2)) + pos.getX();
            z1 = r*(float) (cos(phi2)*cos(theda)) + pos.getZ();
            z2 = r*(float) (cos(phi)*cos(theda2)) + pos.getZ();
            z3 = r*(float) (cos(phi2)*cos(theda2)) + pos.getZ();
            vertices.push_back(Vec3(x0,y0,z0));
            vertices.push_back(Vec3(x2,y0,z2));
            vertices.push_back(Vec3(x1,y1,z1));
            vertices.push_back(Vec3(x1,y1,z1));
            vertices.push_back(Vec3(x2,y0,z2));
            vertices.push_back(Vec3(x3,y1,z3));

            index.push_back(k++ );
            index.push_back(k++);
            index.push_back(k++);
            index.push_back(k++);
            index.push_back(k++);
            index.push_back(k++);

//update x0,z0, theda
            theda = theda2;
            x0 = x2; z0 = z2;

            uvs.push_back(Vec2(u0, v0));
            uvs.push_back(Vec2(u0+du, v0));
            uvs.push_back(Vec2(u0, v0+dv));
            uvs.push_back(Vec2(u0, v0+dv));
            uvs.push_back(Vec2(u0+du, v0));
            uvs.push_back(Vec2(u0+du, v0+dv));
            u0 += du;

       }
   }

    return true;

}

bool GlText::init(float scale, Vec3 pos, const char* bkgnd)
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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    vector<Vec3> vertices;
    vector<Vec2> uvs;
    vector <unsigned short> ind;
    load(scale, pos, vertices, uvs, ind);

    mMvpMatrixUniform = glGetUniformLocation(mProgramId, "MVP");
    glGenBuffers(3, mVertexBufId);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufId[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vec3), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufId[1]);
    glBufferData(GL_ARRAY_BUFFER, uvs.size()*sizeof(Vec2), &uvs[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVertexBufId[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size()* sizeof(unsigned short), &ind[0], GL_STATIC_DRAW);

    mNumToDraw = ind.size();

    mMatModel = Mat4::Identity();

}

void GlText::transform(Mat4& transform)
{
    mMatModel = mMatModel * transform;
}
void GlText::update(Mat4& pojection, Mat4& view, Vec3& light)
{
    mMatMvp = pojection * view * mMatModel;



}

void GlText::cleanup()
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

void GlText::draw(bool bReload)
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
