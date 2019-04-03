#ifndef SPEEDMETER_H
#define SPEEDMETER_H
#include "cluster.h"
class Disc;
class Needle;
class Disc;

class SpeedMeter
{
public:
    SpeedMeter();
    ~SpeedMeter();
    virtual bool init();
    virtual void cleanup();
    /* \brief update view pointer
     * \param pojection
     */
    virtual void update(Mat4& pojection, Mat4& view, Vec3& light);

    virtual void setSpeed(int kmph);
    virtual void setFuel(float level);
    /* \brief render object
     * \param bReload set true to reload texture
     *
     */
    virtual void draw(bool bReload = false);
protected:
    Disc mDisc;
    Needle mMainNeedle;
    Pin mPin;
};

class RpmMeter {
public:
    RpmMeter();
    ~RpmMeter();
    virtual bool init();
    virtual void cleanup();
    /* \brief update view pointer
     * \param pojection
     */
    virtual void update(Mat4& pojection, Mat4& view, Vec3& light);

    virtual void setRpm(int rpm);
    virtual void setTemp(float degree);
    /* \brief render object
     * \param bReload set true to reload texture
     *
     */
    virtual void draw(bool bReload = false);
protected:
    Disc mDisc;
    Needle mMainNeedle;
    Pin mPin;

};

#endif // SPEEDMETER_H
