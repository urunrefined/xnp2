#ifndef VERTICES_H
#define VERTICES_H

#include <vector>
#include <iostream>
#include <iomanip>

namespace BR {

class __attribute__((packed)) Vec2 {
public:
	float x, y;

	Vec2 operator-(const Vec2& vec) const;
	void operator-=(const Vec2& vec);
	Vec2 operator+(const Vec2& vec) const;
	void operator+=(const Vec2& vec);
	Vec2 operator*(float multiplicator);
	void operator*=(float multiplicator);

	void print() const;

	float getLength() const;

	Vec2& normalize();

	void getData(std::vector<float>& vec) const;

};

class __attribute__((packed)) Vec3 {
public:
	float x;
	float y;
	float z;

	static Vec3 cross
	(const Vec3& vec, const Vec3& vec2, const Vec3& vec3);

	bool operator==(const Vec3& a) const;
	Vec3 operator+(const Vec3& vec) const;
	void operator+=(const Vec3& vec);
	Vec3 operator-(const Vec3& vec) const;
	void operator-=(const Vec3& vec);
	Vec3 operator/(float divisor) const;
	void operator/=(float divisor);
	Vec3 operator*(float multiplicator) const;
	void operator*=(float multiplicator);

	float dot(const Vec3& vec) const;

	Vec3 cross (const Vec3& vec) const;

	Vec3& normalize();

	float getLength() const;

	void print() const;

	void mult(const float *matrix);

	void mult (float scalar);

	void getData(std::vector<float>& vec) const;

	Vec3& negate();

};

class __attribute__((packed)) Vec4{
public:
	float x, y, z, w;

	void print() const {
		std::cout << std::setw(4) << "XYZW: " << std::setw(10) << x <<
			std::setw(10) << y << std::setw(10) << z <<
			std::setw(10) << w<< std::endl;
	}

};

using Vertex3D = Vec3;
using Vertex4D = Vec4;

using Vector3D = Vec3;
using Vector4D = Vec4;

using Color3D  = Vec3;
using Color4D  = Vec4;

}

#endif // VERTICES_H
