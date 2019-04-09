#ifndef GLTEXT_H
#define GLTEXT_H

#include <vector>
#include <GL/glew.h>
#include "lib/vecmath.h"

using namespace std;
class GlText
{
public:
    GlText();
     ~GlText();
    virtual bool init(float scale, Vec3 pos, const char* bkgnd);
    virtual void cleanup();

    /* \brief update view pointer
     * \param pojection
     */
    virtual void update(Mat4& pojection, Mat4& view, Vec3& light);

    /* \brief transform object locally
     * \param transform
     */
    virtual void transform(Mat4& transform);

    /* \brief render object
     * \param bReload set true to reload texture
     *
     */
    virtual void draw(bool bReload = false);

protected:
    bool load(float r, Vec3& pos, vector < Vec3 > & vertices, vector <Vec2 > & uvs, vector < unsigned short > & inddex);

    /*<! OpenGL ID */
    GLuint mProgramId;          /*<! Shader program ID */
    //GLuint mVertexAttrib;      /*<! ID of Vertex position attribute, used in OpenGL ES */
    //GLuint mColorAttrib;         /*<! ID of texture attrib in shader program */

    GLuint mMvpMatrixUniform;        /*<! ID of matrix uniform in shader program */
    GLuint mVaoId;              /*<! ID of Vertex array object*/
    GLuint mVertexBufId[3];     /*<! buffer ID: 0=Vertex, 1=color buffer */

    GLuint mNumToDraw;    /*<! number of elements to draw */

    GLuint mTextureData;        /*<! texture image data id */
    GLuint mTextureUniform;     /*<! texture uniform id */

private:
    Mat4    mMatModel;
    Mat4    mMatMvp;
};

#endif // GLTEXT_H
