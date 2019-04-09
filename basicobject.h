#ifndef BasicObject_H
#define BasicObject_H

#include <GL/glew.h>
#include "lib/vecmath.h"

class BasicObject
{
public:
    BasicObject();
    ~BasicObject();
    virtual bool init();
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
    /*<! OpenGL ID */
    GLuint mProgramId;          /*<! Shader program ID */
    GLuint mVertexAttrib;      /*<! ID of Vertex position attribute */
    GLuint mColorAttrib;         /*<! ID of texture attrib in shader program */

    GLuint mMvpMatrixUniform;        /*<! ID of matrix uniform in shader program */
    GLuint mVaoId;              /*<! ID of Vertex array object*/
    GLuint mVertexBufId[3];     /*<! buffer ID: 0=Vertex, 1=UV texture, buffer */

     unsigned int mNumToDraw;    /*<! number of elements to draw */

private:
    Mat4    mMatModel;
    Mat4    mMatMvp;
};


#endif // BasicObject_H
