//
// Created by droc101 on 1/13/25.
//

#include "Vector2.h"
#include <math.h>
#include "../defines.h"

Vector2 v2(const double x, const double y) // Create a Vector2
{
    Vector2 v;
    v.x = x;
    v.y = y;
    return v;
}

Vector2 v2s(const double xy) // Create a Vector2 with x and y set the same
{
    Vector2 v;
    v.x = xy;
    v.y = xy;
    return v;
}

double Vector2Distance(const Vector2 a, const Vector2 b) // Get the distance between two Vector2s
{
    const double dx = a.x - b.x;
    const double dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}

double Vector2DistanceToLine(const Vector2 lineStart, const Vector2 lineEnd, const Vector2 testPoint)
{
	double lineMag = Vector2Distance(lineEnd, lineStart);

	if (lineMag == 0.0)
	{
		return Vector2Distance(lineStart, testPoint);
	}

	double u = (((testPoint.x - lineStart.x) * (lineEnd.x - lineStart.x)) +
				((testPoint.y - lineStart.y) * (lineEnd.y - lineStart.y))) / (lineMag * lineMag);

	if (u < 0.0)
	{
		return Vector2Distance(testPoint, lineStart);
	}
	if (u > 1.0)
	{
		return Vector2Distance(testPoint, lineEnd);
	}

	Vector2 intersection;
	intersection.x = lineStart.x + u * (lineEnd.x - lineStart.x);
	intersection.y = lineStart.y + u * (lineEnd.y - lineStart.y);

	return Vector2Distance(testPoint, intersection);
}


double Vector2Length(const Vector2 vec)
{
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

Vector2 Vector2Normalize(const Vector2 vec)
{
    const double len = Vector2Length(vec);
    if (len == 0)
    {
        return v2(0, 0);
    }
    return v2(vec.x / len, vec.y / len);
}

Vector2 Vector2FromAngle(const double angle)
{
    return v2(cos(angle), sin(angle));
}

Vector2 Vector2Add(const Vector2 a, const Vector2 b)
{
    return v2(a.x + b.x, a.y + b.y);
}

Vector2 Vector2Sub(const Vector2 vec, const Vector2 offset)
{
    return v2(vec.x - offset.x, vec.y - offset.y);
}

Vector2 Vector2Scale(const Vector2 vec, const double scale)
{
    return v2(vec.x * scale, vec.y * scale);
}


double Vector2Dot(const Vector2 a, const Vector2 b)
{
    return a.x * b.x + a.y * b.y;
}

Vector2 Vector2Rotate(const Vector2 vec, const double angle)
{
    const double cosAngle = cos(angle);
    const double sinAngle = sin(angle);

    return v2(vec.x * cosAngle - vec.y * sinAngle, vec.x * sinAngle + vec.y * cosAngle);
}

double Vector2Angle(const Vector2 a, const Vector2 b)
{
    return acos(Vector2Dot(a, b) / (Vector2Length(a) * Vector2Length(b)));
}

Vector2 Vector2Div(const Vector2 vec, const double divisor)
{
    return v2(vec.x / divisor, vec.y / divisor);
}
