/*<! basic data type re-definition */
#ifndef _NFORE_DATA_TYPE___
#define _NFORE_DATA_TYPE___

typedef unsigned char nfByte, *nfPByte;

typedef struct _nfFloat2D{
    float x;
    float y;
}nfFloat2D;

typedef struct _nfFloat3D{
    float x;
    float y;
    float z;
}nfFloat3D;

typedef struct _nfRectF {
    float l;
    float t;
    float r;
    float b;
}nfRectF;


#define SAFE_FREE(p) if(p) { free(p); p=NULL;}
#define SAFE_ALLOC(p, n) { if(p) free(p); p=(typeof(p)) malloc(n);}

class nfImage {
public:
static nfImage* create(unsigned int w, unsigned int h, unsigned int bpp);
///
/// \brief ref create a nfImage object associated with existed data. It doesn't own the data,
/// so the bufer is not destroed when nfImage is destroyed.
/// \param data
/// \param w
/// \param h
/// \param bpp
/// \return the nfImage object
///
static nfImage* ref(unsigned char* data, unsigned int w, unsigned int h, unsigned int stride);

////
/// \brief clone create a nfImage object but share the same buffer as source. The new object owns the buffer, so the source should use detach() to release the buffer
/// \param source
/// \return pointer to the clone nfImage
///
static nfImage* clone(nfImage* pSource);

static void destroy(nfImage** ppImage);
///
/// \brief dettach the buffer from a nfImage object and destroy this object
/// \param ppImage object to be destroyed
/// \return the image buffer
///
static nfPByte dettach(nfImage** ppImage);

    unsigned char* buffer;   /*<! image data */
    unsigned int width;  /*<! image width in pixel */
    unsigned int stride; /*<! bytes per scan line */
    unsigned int height; /*<! image height in lines */
    bool bRef; /*!<true: buffer is referenced, don't free */
};

class nfBuffer{
public:
	nfBuffer(){};
	nfBuffer(unsigned int elements);
	virtual ~nfBuffer();
    void set(unsigned int /* offset */, char /*value*/){};
	void* data(){return NULL;}

	virtual	unsigned int length(){return mLength;}
	virtual	unsigned int size(){ return mCounts;}
static nfBuffer* create(unsigned int counts);
static void destroy(nfBuffer** pBuffer);
private:

protected:
    unsigned int mBpu; /*<! bytes of per data unit */
    unsigned int mCounts; /*<! counts of data unit */
    unsigned int mLength;  /*<! length in bytes of the data */
};

class nfFloatBuffer : public nfBuffer {
public:
	nfFloatBuffer(unsigned int elements);
    virtual ~nfFloatBuffer();
	float* data(){ return mpData;}
	void set(unsigned int offset, float value){ mpData[offset] = value;};
static nfFloatBuffer* create(unsigned int counts);
static void destroy(nfFloatBuffer** ppBuffer);
private:
    float* mpData;
};

class nfUshortBuffer : public nfBuffer {
public:
	nfUshortBuffer(unsigned int elements);
    virtual ~nfUshortBuffer();
	void set(unsigned int offset, unsigned short value){ mpData[offset] = value;};
	unsigned short* data(){ return mpData;}
static nfUshortBuffer* create(unsigned int counts);
static void destroy(nfUshortBuffer** ppBuffer);
private:
    unsigned short* mpData;
};
#endif //_NFORE_DATA_TYPE___
