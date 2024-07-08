#ifndef Rectangle2DPersistenceUtils_H
#define Rectangle2DPersistenceUtils_H

#include <string>

#include "Rectangle2D.h"

#include "SimplePersistenceUtils.h"

namespace Rectangle2DPersistenceUtils
{
    SaveOperationResult saveIntRectangle2D(const std::string& persistenceFilename, const Rectangle2D<int>& rectangle, const std::string& directory = "");
    LoadOperationResult loadIntRectangle2D(const std::string& persistenceFilename, Rectangle2D<int>& rectangle, const std::string& directory = "");
}

#endif