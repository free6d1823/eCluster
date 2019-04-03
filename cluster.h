#ifndef CLUSTER_H
#define CLUSTER_H

#include <vector>
#include <GL/glew.h>
#include "lib/vecmath.h"

using namespace std;
class Needle
{
public:
    Needle();
    ~Needle();
    virtual bool init(float scale, Vec3 pos);
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
    virtual void setRpm(int rpm);

protected:  
    /*<! OpenGL ID */
    GLuint mProgramId;          /*<! Shader program ID */
    //GLuint mVertexAttrib;      /*<! ID of Vertex position attribute, used in OpenGL ES */
    //GLuint mColorAttrib;         /*<! ID of texture attrib in shader program */

    GLuint mMvpMatrixUniform;        /*<! ID of matrix uniform in shader program */
    GLuint mVaoId;              /*<! ID of Vertex array object*/
    GLuint mVertexBufId[3];     /*<! buffer ID: 0=Vertex, 1=color buffer */

    GLuint mNumToDraw;    /*<! number of elements to draw */

private:
    Mat4    mMatModel;
    Mat4    mMatRotate;
    Mat4    mMatMvp;
};

class Pin
{
public:
    Pin();
    ~Pin();
    virtual bool init(float scale, Vec3 pos);
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
    virtual void setLevel(float level);

protected:
    /*<! OpenGL ID */
    GLuint mProgramId;          /*<! Shader program ID */
    //GLuint mVertexAttrib;      /*<! ID of Vertex position attribute, used in OpenGL ES */
    //GLuint mColorAttrib;         /*<! ID of texture attrib in shader program */

    GLuint mMvpMatrixUniform;        /*<! ID of matrix uniform in shader program */
    GLuint mVaoId;              /*<! ID of Vertex array object*/
    GLuint mVertexBufId[3];     /*<! buffer ID: 0=Vertex, 1=color buffer */

    GLuint mNumToDraw;    /*<! number of elements to draw */

private:
    Mat4    mMatModel;
    Mat4    mMatRotate;
    Mat4    mMatMvp;
};

class Disc
{
public:
    Disc();
    ~Disc();
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
    bool load(float r, float tx, vector < Vec3 > & vertices, vector <Vec2 > & uvs, vector < unsigned short > & inddex);

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


#endif // CLUSTER_H
