#include "speedmeter.h"
#define METER1_POS_X -2
#define METER2_POS_X 2

#define MAX_RPM 6000
#define MIN_RPM 0
#define MAX_SPEED 260
#define MIN_SPEED 0
#define MAX_TEMP 180
#define MIN_TEMP 0
#define MAX_FUEL 100
#define MIN_FUEL 0
SpeedMeter::SpeedMeter()
{

}

SpeedMeter :: ~SpeedMeter()
{

}
bool SpeedMeter :: init()
{
    mDisc.init(1, Vec3(METER2_POS_X,0,0), "speed2.png");
    mMainNeedle.init(1, Vec3(METER2_POS_X,0,0));
    mPin.init(1, Vec3(METER2_POS_X,0,0));
    return true;
}
void SpeedMeter :: cleanup()
{
    mDisc.cleanup();
    mMainNeedle.cleanup();
    mPin.cleanup();
}

void SpeedMeter :: update(Mat4& pojection, Mat4& view, Vec3& light)
{
    mDisc.update(pojection, view, light);
    mMainNeedle.update(pojection, view, light);
    mPin.update(pojection, view, light);
}
void SpeedMeter :: setSpeed(int kmph)
{
    //convert kmph to 0~10000
    int rpm = 10000 * kmph/(MAX_SPEED - MIN_SPEED);
    mMainNeedle.setRpm(rpm);
}
void SpeedMeter :: setFuel(float level)
{
    float degree = 100.0f*(level - MIN_FUEL)/(MAX_FUEL-MIN_FUEL);
    mPin.setLevel(degree);
}

void SpeedMeter ::draw(bool bReload)
{
    mDisc.draw(bReload);
    mMainNeedle.draw(bReload);
    mPin.draw(bReload);
}

//////////////////////////////////////
/////////////////////////////////////////////
//

RpmMeter :: RpmMeter()
{

}
RpmMeter :: ~RpmMeter()
{

}
bool RpmMeter :: init()
{
    mDisc.init(1, Vec3(METER1_POS_X,0,0), "speed1.png");
    mMainNeedle.init(1, Vec3(METER1_POS_X,0,0));
    mPin.init(1, Vec3(METER1_POS_X,0,0));
    return true;
}
void RpmMeter :: cleanup()
{
    mDisc.cleanup();
    mMainNeedle.cleanup();
    mPin.cleanup();
}

void RpmMeter :: update(Mat4& pojection, Mat4& view, Vec3& light)
{
    mDisc.update(pojection, view, light);
    mMainNeedle.update(pojection, view, light);
    mPin.update(pojection, view, light);
}
void RpmMeter :: setRpm(int rpm)
{
    int rpm2 = 10000 * (rpm-MIN_RPM)/(MAX_RPM - MIN_RPM);

    mMainNeedle.setRpm(rpm2);
}
void RpmMeter :: setTemp(float degree)
{
    float percent = 100.0f*(degree - MIN_TEMP)/(MAX_TEMP-MIN_TEMP);

    mPin.setLevel(percent);
}

void RpmMeter ::draw(bool bReload)
{
    mDisc.draw(bReload);
    mMainNeedle.draw(bReload);
    mPin.draw(bReload);
}
