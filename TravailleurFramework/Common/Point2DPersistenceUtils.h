#ifndef Point2DPersistenceUtils_H
#define Point2DPersistenceUtils_H

#include <string>

#include "Point2D.h"

#include "SimplePersistenceUtils.h"

namespace Point2DPersistenceUtils
{
    SaveOperationResult saveIntPoint2D(const std::string& persistenceFilename, const Point2D<int>& point, const std::string& directory = "");
    LoadOperationResult loadIntPoint2D(const std::string& persistenceFilename, Point2D<int>& point, const std::string& directory = "");
}

#endif