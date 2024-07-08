#ifndef Point2D_H
#define Point2D_H

#include <string>

#include "SimplePersistenceUtils.h"

template<typename T>
class Point2D
{
    public:
    Point2D() = default;
    Point2D(const T& x, const T& y): x(x), y(y)
    {
    }
    Point2D& operator=(const Point2D& other)
    {
        this->x = other.x;
        this->y = other.y;
        return *this;
    }
    bool operator==(const Point2D& other) const
    {
        return this->x == other.x && this->y == other.y;
    }
    bool operator!=(const Point2D& other) const
    {
        return !(*this == other);
    }
    bool operator<(const Point2D& other) const
    {
        return this->x < other.x && this->y < other.y;
    }
    void operator+=(const Point2D& other)
    {
        this->x += other.x;
        this->y += other.y;
    }
    Point2D operator+(const Point2D& other) const
    {
        return Point2D(this->x + other.x, this->y + other.y);
    }
    void operator-=(const Point2D& other)
    {
        this->x -= other.x;
        this->y -= other.y;
    }
    Point2D operator-(const Point2D& other) const
    {
        return Point2D(this->x - other.x, this->y - other.y);
    }

    const T& getX() { return this->x; }
    const T& getY() { return this->y; }

    T x = 0;
    T y = 0;
};

#endif