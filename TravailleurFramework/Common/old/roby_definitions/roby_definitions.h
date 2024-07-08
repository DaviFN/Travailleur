/*
    some common definitions for Travailleur
*/

#ifndef ROBY_DEFINITIONS_H
#define ROBY_DEFINITIONS_H

#define SCREEN_PROPORTION (100)
#define GAME_SCREEN_WIDTH ((1280 * SCREEN_PROPORTION)/100)
#define GAME_SCREEN_HEIGHT ((720 * SCREEN_PROPORTION)/100)

#include <assert.h>


#include "TravailleurFramework\3rdparty\EasyLogging\easylogging++.h"



// flags for debugging purposes
//#define TRACE_STRATEGIC_POINTS
//#define TRACE_LOOP_STRATEGIC_POINTS

//#define DEBUG_TREELIKESTRUCTURE
//#define DEBUG_RECTANGULARREGION
//#define DEBUG_ROBY_CONTEXT

//#define DEBUG_GAME_CLASS

//#define DEBUG_CALLBACK_PS_ZOOM_2000

#include <string>
#include <vector>
#include <cstdio>
#include <math.h>


const std::string travailleurDataPath = "C:\\Users\\davif\\Desktop\\travailleur\\travailleurData";

#define MAX_COLOR_DISTANCE 441.67295594

struct point
{
    point(){};
    point(int x, int y)
    {
        this->x = x;
        this->y = y;
    }
    int x;
    int y;
    void operator +=(const point& other)
    {
        this->x += other.x;
        this->y += other.y;
    }
    point operator +(const point& other)
    {
        point returnPoint;
        returnPoint.x = this->x + other.x;
        returnPoint.y = this->y + other.y;
        return returnPoint;
    }
    point operator -(const point& other)
    {
        point returnPoint;
        returnPoint.x = this->x - other.x;
        returnPoint.y = this->y - other.y;
        return returnPoint;
    }
};

#include "TravailleurFramework\Common\SimpleTimer\SimpleTimer.h"
#include "TravailleurFramework\Common\DebugTimer\DebugTimer.h"
#include "TravailleurFramework\Common\FileLog\FileLog.h"

//extern FileLog* debugFile;

#endif