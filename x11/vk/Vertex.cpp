#include "Vertex.h"
#include <math.h>

namespace BR {

Vec2 Vec2::operator-(const Vec2 &vec) const {
	return Vec2 {x - vec.x, y - vec.y};
}

void Vec2::operator-=(const Vec2 &vec){
	*this = *this - vec;
}

Vec2 Vec2::operator+(const Vec2 &vec) const {
	return Vec2 {x + vec.x, y + vec.y};
}

void Vec2::operator+=(const Vec2 &vec){
	*this = *this + vec;
}

Vec2 Vec2::operator*(float multiplicator){
	return Vec2 {x * multiplicator, y * multiplicator};
}

void Vec2::operator*=(float multiplicator){
	*this = *this * multiplicator;
}

void Vec2::print() const {
	std::cout << std::setw(4) << "XY: " << std::setw(10) << x <<
				 std::setw(16) << y << std::endl;
}

float Vec2::getLength() const {
	return (float)sqrt(x * x + y * y);
}

Vec2 &Vec2::normalize() {
	float length = getLength();
	if(length){
		x /= length;
		y /= length;
	}

	return *this;
}

void Vec2::getData(std::vector<float> &vec) const {
	vec.emplace_back(x);
	vec.emplace_back(y);
}

Vec3 Vec3::cross(const Vec3 &vec, const Vec3 &vec2, const Vec3 &vec3){
	Vec3 vec13 = vec3;
	Vec3 vec12 = vec2;
	vec13 -= (vec);
	vec12 -= (vec);

	Vec3 cr {
		vec12.y * vec13.z - vec12.z * vec13.y,
				vec12.z * vec13.x - vec12.x * vec13.z,
				vec12.x * vec13.y - vec12.y * vec13.x,
	};

	return cr.normalize();
}

bool Vec3::operator==(const Vec3 &a) const{
	return a.x == x && a.y == y && a.z == z;
}

Vec3 Vec3::operator+(const Vec3 &vec) const {
	return {x + vec.x, y + vec.y, z + vec.z};
}

void Vec3::operator+=(const Vec3 &vec){
	*this = *this + vec;
}

Vec3 Vec3::operator-(const Vec3 &vec) const {
	return {x - vec.x, y - vec.y, z - vec.z};
}

void Vec3::operator-=(const Vec3 &vec){
	*this = *this - vec;
}

Vec3 Vec3::operator/(float divisor) const {
	if(divisor != 0.0f)
		return {x / divisor, y / divisor, z / divisor};

	return *this;
}

void Vec3::operator/=(float divisor){
	*this = *this / divisor;
}

Vec3 Vec3::operator*(float multiplicator) const{
	return {x * multiplicator, y * multiplicator, z * multiplicator};
}

void Vec3::operator*=(float multiplicator){
	*this = *this * multiplicator;
}

float Vec3::dot(const Vec3 &vec) const {
	return x * vec.x + y * vec.y + z * vec.z;
}

Vec3 Vec3::cross(const Vec3 &vec) const {
	return {
		y * vec.z - z * vec.y,
				z * vec.x - x * vec.z,
				x * vec.y - y * vec.x,
	};
}

Vec3 &Vec3::normalize() {
	float length = getLength();
	if(length != 0.0f){
		x /= length;
		y /= length;
		z /= length;
	}

	return *this;
}

float Vec3::getLength() const {
	return (float)sqrt(x * x + y * y + z * z);
}

void Vec3::print() const {
	std::cout << std::setw(4) << "XYZ: " << std::setw(10) << x <<
				 std::setw(16) << y << std::setw(22) << z << std::endl;
}

void Vec3::mult(const float *matrix){
	*this = Vec3 {
		matrix[0] * x + matrix[1] * y + matrix[2] *  z + matrix[3],
				matrix[4] * x + matrix[5] * y + matrix[6] *  z + matrix[7],
				matrix[8] * x + matrix[9] * y + matrix[10] * z + matrix[11]
				//matrix[12] * x + matrix[13] * y + matrix[14] * z + matrix[15] * 1,
	};
}

void Vec3::mult(float scalar){
	x *= scalar;
	y *= scalar;
	z *= scalar;
}

void Vec3::getData(std::vector<float> &vec) const {
	vec.emplace_back(x);
	vec.emplace_back(y);
	vec.emplace_back(z);
}

Vec3 &Vec3::negate(){
	x = -x;
	y = -y;
	z = -z;

	return *this;
}

}
