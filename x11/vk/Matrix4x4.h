#ifndef MATRIX4X4_H_
#define MATRIX4X4_H_

#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <string>
#include "Vertex.h"

namespace BR {


class Matrix4x4f {
private:
	float data[16];
	
	inline Matrix4x4f mult(const Matrix4x4f& right);

public:

	/*Create matrix with undefined content, does not
	 *set anything*/
	Matrix4x4f();
	
	/*create matrix with 16 elements, unsafe*/
	Matrix4x4f(const float *elements);
	
	float& operator[](size_t elem);
	float& operator()(size_t x, size_t y);
	
	Vec4 operator*(const Vec4& v) const;
	Matrix4x4f operator*(const Matrix4x4f& matrix);
	
	/*create matrix with elements set to element*/
	Matrix4x4f(float& element);
	~Matrix4x4f();
	
	void appendTo(std::vector<float>& vec) const;
	
	inline void setToIdentity();
	inline Matrix4x4f& zero();
	
	inline void mult (float scalar);
	
	void translate(float x, float y, float z);
	void setTranslation(const Vec3& vec);
	void setTranslation(const Vec2& vec);
	void setScale(const Vec3& vec);
	
	Vec3 getVec3() const;
	Vec2 getVec2() const;
	
	static Matrix4x4f ortho1To1();
	static Matrix4x4f ortho16To9();
	
	const float* getData() const;
	
	void print() const;
	
	static Matrix4x4f ident();
	
	float& tx();
	float& ty();
	float& tz();
	float& sx();
	float& sy();
	float& sz();
};

}


#endif /* MATRIX4X4_H_ */
