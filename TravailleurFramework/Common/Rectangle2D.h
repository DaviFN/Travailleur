#ifndef Rectangle2D_H
#define Rectangle2D_H

#include "Point2D.h"

template<typename T>
class Rectangle2D
{
    public:
    Rectangle2D() = default;
    Rectangle2D(const Rectangle2D<T>& other) :
        topLeft(other.topLeft),
        size(other.size)
    {
    }
    Rectangle2D(const Point2D<T>& topLeft, const Point2D<T>& size):
        topLeft(topLeft),
        size(size)
    {
    }
    Rectangle2D& operator=(const Rectangle2D& other)
    {
        this->topLeft = other.topLeft;
        this->size = other.size;
        return *this;
    }
    bool operator==(const Rectangle2D& other) const
    {
        return this->topLeft == other.topLeft && this->size == other.size;
    }
    bool operator!=(const Rectangle2D& other) const
    {
        return !(*this == other);
    }
    bool operator<(const Rectangle2D& other) const
    {
        return this->topLeft < other.topLeft && this->size < other.size;
    }

    inline int getWidth() const
    {
        return abs(size.x);
    }
    inline int getHeight() const
    {
        return abs(size.y);
    }

    inline int getArea() const
    {
        return getWidth() * getHeight();
    }

    inline bool isValid() const
    {
        return size.x > 0 && size.y > 0;
    };
    inline bool isInsideOther(const Rectangle2D<T>& other) const
    {
        return topLeft.x >= other.topLeft.x && topLeft.y >= other.topLeft.y && topLeft.x + size.x <= other.topLeft.x + other.size.x && topLeft.y + size.y <= other.topLeft.y + other.size.y;
    }
    inline Rectangle2D<T> keepOnlyPartInsideOther(const Rectangle2D<T>& other)
    {
        const Point2D<T> newTopLeft = Point2D<T>(this->topLeft.x < other.topLeft.x ? other.topLeft.x : this->topLeft.x, this->topLeft.y < other.topLeft.y ? other.topLeft.y : this->topLeft.y);
        const Point2D<T> bottomRight = this->getBottomRight();
        const Point2D<T> otherBottomRight = other.getBottomRight();
        const Point2D<T> newBottomRight = Point2D<T>(bottomRight.x > otherBottomRight.x ? otherBottomRight.x : bottomRight.x, bottomRight.y > otherBottomRight.y ? otherBottomRight.y : bottomRight.y);
        const Point2D<T> newSize = newBottomRight - newTopLeft;
        Rectangle2D<T> result = Rectangle2D<T>(newTopLeft, newSize);
        result.assureSizeIsNonNegative();
        return result;
    }
    inline bool pointIsInside(const Point2D<T> point) const
    {
        const Point2D<T> bottomRight = getBottomRight();
        return point.x >= topLeft.x && point.x <= bottomRight.x && point.y >= topLeft.y && point.y <= bottomRight.y;
    }
    inline void assureSizeIsNonNegative()
    {
        if (size.x < 0) size.x = 0;
        if (size.y < 0) size.y = 0;
    }

    inline const Point2D<T>& getTopLeft() const { return this->topLeft; }
    inline Point2D<T> getTopRight() const { return this->topLeft + Point2D<T>(this->size.x, 0); }
    inline Point2D<T> getBottomLeft() const { return this->topLeft + Point2D<T>(0, this->size.y); }
    inline Point2D<T> getBottomRight() const { return this->topLeft + Point2D<T>(this->size.x, this->size.y); }
    inline const Point2D<T>& getSize() const { return this->size; }

    Point2D<T> topLeft;
    Point2D<T> size;
};

#endif