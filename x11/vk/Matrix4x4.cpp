#include "Matrix4x4.h"
#include "string.h"

namespace BR {

Matrix4x4f Matrix4x4f::mult(const Matrix4x4f &right){
	unsigned int i,j,k;
	Matrix4x4f out;

	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			out.data[i+(j*4)] = 0;
			for(k=0;k<4;k++){
				/*
					cout << i+(j*4) << " ("<< out.data[i+(j*4)] << ") += (" <<
					k + j * 4  << ") " << left.data [k + j * 4] << " (" <<
					i + k * 4 << ") " << right.data[i + k * 4] << endl;*/

				out.data[i+(j*4)] +=
						data [k + j * 4] *
						right.data[i + k * 4];
			}
		}
	}

	return out;
}

Matrix4x4f::Matrix4x4f(){}

Matrix4x4f::Matrix4x4f(const float *elements){
	unsigned int i;
	for(i=0; i<16; i++){
		data[i] = elements[i];
	}
}

float &Matrix4x4f::operator[](size_t elem){
	return data[elem];
}

float &Matrix4x4f::operator()(size_t x, size_t y){
	return data[x + y * 4];
}

Vec4 Matrix4x4f::operator*(const Vec4 &v) const {
	return Vec4{
		data[0]  * v.x + data[1]  * v.y + data[2]  * v.z + data[3]  * v.w,
				data[4]  * v.x + data[5]  * v.y + data[6]  * v.z + data[7]  * v.w,
				data[8]  * v.x + data[9]  * v.y + data[10] * v.z + data[11] * v.w,
				data[12] * v.x + data[13] * v.y + data[14] * v.z + data[15] * v.w
	};
}

Matrix4x4f Matrix4x4f::operator*(const Matrix4x4f &matrix) {
	return mult(matrix);
}

Matrix4x4f::Matrix4x4f(float &element){
	unsigned int i;
	for(i=0; i<16; i++){
		data[i] = element;
	}
}

Matrix4x4f::~Matrix4x4f(){}

void Matrix4x4f::appendTo(std::vector<float> &vec) const {
	for(const float& datum : data){
		vec.push_back(datum);
	}
}

void Matrix4x4f::setToIdentity(){
	zero();

	data[0] = (float)1;
	data[5] = (float)1;
	data[10] = (float)1;
	data[15] = (float)1;
}

Matrix4x4f &Matrix4x4f::zero(){
	memset(data, 0, sizeof(data));
	return *this;
}

void Matrix4x4f::mult(float scalar) {
	size_t i;

	for(i=0;i<16;i++)
		data[i] *= scalar;
}

void Matrix4x4f::translate(float x, float y, float z){
	data[3]  += x;
	data[7]  += y;
	data[11] += z;
}

void Matrix4x4f::setTranslation(const Vec3 &vec){
	data[3]  = vec.x;
	data[7]  = vec.y;
	data[11] = vec.z;
}

void Matrix4x4f::setTranslation(const Vec2 &vec){
	data[3]  = vec.x;
	data[7]  = vec.y;
}

void Matrix4x4f::setScale(const Vec3 &vec){
	data[0]  = vec.x;
	data[5]  = vec.y;
	data[10] = vec.z;
}

Vec3 Matrix4x4f::getVec3() const {
	return {data[3], data[7], data[11]};
}

Vec2 Matrix4x4f::getVec2() const {
	return Vec2 {data[3], data[7]};
}

Matrix4x4f Matrix4x4f::ortho1To1(){
	Matrix4x4f matrix;

	matrix.zero();
	matrix(0, 0) =  2.0f;
	matrix(1, 1) = -2.0f;
	matrix(2, 2) =  0;

	matrix(3, 0) = -1.0f;
	matrix(3, 1) =  1.0f;
	matrix(3, 2) =  0;

	matrix(3, 3) =  1.0f;

	return matrix;
}

Matrix4x4f Matrix4x4f::ortho16To9(){
	Matrix4x4f matrix = ortho1To1();
	matrix(0,0) = matrix(0,0) / (16.0f/9.0f);

	return matrix;
}

const float *Matrix4x4f::getData() const {
	return data;
}

void Matrix4x4f::print() const {
	unsigned int i,j;

	std::cout << "Matrix cols: " << 4 << ", rows: " << 4 << std::endl;

	for (i=0;i<4;i++){
		for(j=0;j<4;j++){
			std::cout << std::setw(12);
			std::cout << data[j + i*4] << " ";
		}
		std::cout << std::endl;
	}
}

Matrix4x4f Matrix4x4f::ident(){
	Matrix4x4f matrix;
	matrix.setToIdentity();
	return matrix;
}

float &Matrix4x4f::tx(){
	return data[3];
}

float &Matrix4x4f::ty(){
	return data[7];
}

float &Matrix4x4f::tz(){
	return data[11];
}

float &Matrix4x4f::sx(){
	return data[0];
}

float &Matrix4x4f::sy(){
	return data[5];
}

float &Matrix4x4f::sz(){
	return data[10];
}


}
