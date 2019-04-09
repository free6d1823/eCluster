// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <common/shader.hpp>
#include "lib/vecmath.h"

#include "lib/datatype.h"
#include "lib/GlHelper.h"
#include "cubic.h"
#include "speedmeter.h"
#include "sphere.h"
#include "gltext.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT   600

GlText mText;
Sphere mBkgnd;
RpmMeter   mObject1;
SpeedMeter mObject2;

/////////////////////////////////
/// \brief control items
///
#define MIN_RPM 0
#define MAX_RPM 6000
int gRpm = MIN_RPM;
#define MIN_SPEED 0
#define MAX_SPEED 260
float gSpeed = 0;

#define MIN_TEMP 0
#define MAX_TEMP 180
float gTemp = 25;
#define MAX_FUEL    100
#define MIN_FUEL    0
float gFuel = 100;

Mat4 mMatProj;
Mat4 mMatView;

Vec3 mPosLight = Vec3(4,4,4);

bool bSimulation = false;
double gRpmStep = 5.0;
double gTempStep = 0.0;



void computeMatricesFromInputs2(GLFWwindow* window)
{
    static Vec3 posCam = Vec3( 0, 0, 5 );
    static Vec3 posLookAt = Vec3( 0, 0, 0 );
    static Vec3 camUp = Vec3( 0, 1, 0 );

    static float mFoV = 60;
    static float mAspectRatio = (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT;
    static bool firstTime = false;

    if(!firstTime) {
        glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL );
        glfwSetCursorPos(window, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
        firstTime = true;
    }
    if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
        posCam.setZ(posCam.getZ() - 0.1);
    }
    // Move backward
    else if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
        posCam.setZ(posCam.getZ() + 0.1);
    }
    // Strafe right
    float rotateSpeed = 0.02;
    if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
        posCam.rotateY(rotateSpeed);
    }
    // Strafe left
    else if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
        posCam.rotateY(-rotateSpeed);
    }


    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
   mMatProj = Mat4::Perspective(55.0f*3.14f/180.0f,mAspectRatio, 0.1f, 20.0f);

    //mMatProject = Mat4::Perspective(M_PI*mFoV/180, mAspectRatio, 0.1f, 100.0f);
    // Camera matrix
    // Camera matrix
    mMatView      = Mat4::LookAt(
                                posCam, // Camera is at (4,3,-3), in World Space
                                posLookAt, // and looks at the origin
                                camUp  // Head is up (set to 0,-1,0 to look upside-down)
    );
    // Model matrix : an identity matrix (model will be at the origin)

    if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
        bSimulation =~bSimulation;
        srand(time(NULL));
    }
    if(bSimulation) {
        if (glfwGetKey( window, GLFW_KEY_P ) == GLFW_PRESS){
            gRpmStep-= 0.1;
            if(gRpmStep<0)gRpmStep = 0;

        } else if (glfwGetKey( window, GLFW_KEY_O ) == GLFW_PRESS){
            gRpmStep +=0.1;
            if(gRpmStep>9)gRpmStep = 9;
      }
        if (glfwGetKey( window, GLFW_KEY_T ) == GLFW_PRESS){
            gTempStep = 0.01;
        } else if (glfwGetKey( window, GLFW_KEY_R ) == GLFW_PRESS){
            gTempStep = 0.0;
        }
        else if (glfwGetKey( window, GLFW_KEY_E ) == GLFW_PRESS){
            gTempStep =-0.01;
        }
    }
}


int main( void )
{


    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "eCluster Demo", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.2f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    mBkgnd.init(5, Vec3(0,0,0), "sphere.png");
    mObject1.init();
    mObject2.init();
     // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    do{
        // Measure speed
        double currentTime = glfwGetTime();
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
            // printf and reset
            printf("%f ms/frame\n", 1000.0/double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }

        computeMatricesFromInputs2(window);

        if(bSimulation){
            int a = rand();
            gRpm += ((a%10)-gRpmStep);
            //printf("ran= %d-%d =%d\n", a, gRpmStep, (a%10)-gRpmStep);
            if(gRpm>MAX_RPM)gRpm = MAX_RPM;
            if (gRpm <MIN_RPM) gRpm = MIN_RPM;
            mObject1.setRpm(gRpm);

            gTemp += ((a%10)*gTempStep);
            if(gTemp>MAX_TEMP)gTemp = MAX_TEMP;
            if (gTemp <MIN_TEMP) gTemp = MIN_TEMP;
            mObject1.setTemp(gTemp);

            gSpeed = MAX_SPEED * sin(gRpm*M_PI/(2.0*MAX_RPM));
            if(gSpeed>MAX_SPEED)gSpeed = MAX_SPEED;
            if (gSpeed <MIN_SPEED) gSpeed = MIN_SPEED;
            mObject2.setSpeed((int)gSpeed);

            gFuel -= (float)(gSpeed*gRpm)/(MAX_RPM*MAX_SPEED*100);
            if (gFuel <MIN_FUEL)gFuel  = MIN_FUEL;
            mObject2.setFuel(gFuel);
        }
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mBkgnd.update(mMatProj, mMatView, mPosLight);
        mBkgnd.draw();
        mObject1.update(mMatProj, mMatView, mPosLight);
        mObject1.draw();
        mObject2.update(mMatProj, mMatView, mPosLight);
        mObject2.draw();
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    mBkgnd.cleanup();
    mObject1.cleanup();
    mObject2.cleanup();
    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}
