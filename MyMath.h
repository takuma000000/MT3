#pragma once
#include <cstdint>
#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>
#include <cmath>

/// <summary>
/// 3ŸŒ³ƒxƒNƒgƒ‹
/// </summary>
struct Vector3 final {
	float x;
	float y;
	float z;

	// ‰ÁZ‰‰Zq
	Vector3 operator+(const Vector3& other) const {
		return Vector3(this->x + other.x, this->y + other.y, this->z + other.z);
	}

	// Œ¸Z‰‰Zq
	Vector3 operator-(const Vector3& other) const {
		return Vector3(this->x - other.x, this->y - other.y, this->z - other.z);
	}

	// “àÏ‰‰Zq
	float operator*(const Vector3& other) const {
		return (this->x * other.x + this->y * other.y + this->z * other.z);
	}

	// ƒXƒJƒ‰[æZ‰‰Zq
	Vector3 operator*(float scalar) const {
		return Vector3(this->x * scalar, this->y * scalar, this->z * scalar);
	}

	// ƒXƒJƒ‰[œZ‰‰Zq
	Vector3 operator/(float scalar) const {
		return Vector3(this->x / scalar, this->y / scalar, this->z / scalar);
	}

};

/// <summary>
/// 4x4s—ñ
/// </summary>
struct Matrix4x4 final {
	float m[4][4];

	// ‰ÁZ‰‰Zq
	Matrix4x4 operator+(const Matrix4x4& other) const {
		Matrix4x4 result;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] = this->m[i][j] + other.m[i][j];
			}
		}
		return result;
	}

	// Œ¸Z‰‰Zq
	Matrix4x4 operator-(const Matrix4x4& other) const {
		Matrix4x4 result;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] = this->m[i][j] - other.m[i][j];
			}
		}
		return result;
	}

	// æZ‰‰Zq
	Matrix4x4 operator*(const Matrix4x4& other) const {
		Matrix4x4 result;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] = 0;
				for (int k = 0; k < 4; ++k) {
					result.m[i][j] += this->m[i][k] * other.m[k][j];
				}
			}
		}
		return result;
	}

	// ƒXƒJƒ‰[æZ‰‰Zq
	Matrix4x4 operator*(float scalar) const {
		Matrix4x4 result;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] = this->m[i][j] * scalar;
			}
		}
		return result;
	}

};


Vector3 Add(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return result;
}

Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}

Vector3 Multiply(float scalar, const Vector3& v) {
	Vector3 result;
	result.x = scalar * v.x;
	result.y = scalar * v.y;
	result.z = scalar * v.z;
	return result;
}

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = 0;
			for (int k = 0; k < 4; ++k) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}

	return result;
}


Matrix4x4 MakeRotateXMatrix(float radian) {
	float c = std::cos(radian);
	float s = std::sin(radian);

	Matrix4x4 rotationMatrix;

	rotationMatrix.m[0][0] = 1.0f;
	rotationMatrix.m[0][1] = 0.0f;
	rotationMatrix.m[0][2] = 0.0f;
	rotationMatrix.m[0][3] = 0.0f;
	rotationMatrix.m[1][0] = 0.0f;
	rotationMatrix.m[1][1] = c;
	rotationMatrix.m[1][2] = s;
	rotationMatrix.m[1][3] = 0.0f;
	rotationMatrix.m[2][0] = 0.0f;
	rotationMatrix.m[2][1] = -s;
	rotationMatrix.m[2][2] = c;
	rotationMatrix.m[2][3] = 0.0f;
	rotationMatrix.m[3][0] = 0.0f;
	rotationMatrix.m[3][1] = 0.0f;
	rotationMatrix.m[3][2] = 0.0f;
	rotationMatrix.m[3][3] = 1.0f;

	return rotationMatrix;
}

Matrix4x4 MakeRotateYMatrix(float radian) {
	float c = std::cos(radian);
	float s = std::sin(radian);

	Matrix4x4 rotationMatrix;

	rotationMatrix.m[0][0] = c;
	rotationMatrix.m[0][1] = 0.0f;
	rotationMatrix.m[0][2] = -s;
	rotationMatrix.m[0][3] = 0.0f;
	rotationMatrix.m[1][0] = 0.0f;
	rotationMatrix.m[1][1] = 1.0f;
	rotationMatrix.m[1][2] = 0.0f;
	rotationMatrix.m[1][3] = 0.0f;
	rotationMatrix.m[2][0] = s;
	rotationMatrix.m[2][1] = 0.0f;
	rotationMatrix.m[2][2] = c;
	rotationMatrix.m[2][3] = 0.0f;
	rotationMatrix.m[3][0] = 0.0f;
	rotationMatrix.m[3][1] = 0.0f;
	rotationMatrix.m[3][2] = 0.0f;
	rotationMatrix.m[3][3] = 1.0f;

	return rotationMatrix;
}

Matrix4x4 MakeRotateZMatrix(float radian) {
	float c = std::cos(radian);
	float s = std::sin(radian);

	Matrix4x4 rotationMatrix;

	rotationMatrix.m[0][0] = c;
	rotationMatrix.m[0][1] = s;
	rotationMatrix.m[0][2] = 0.0f;
	rotationMatrix.m[0][3] = 0.0f;
	rotationMatrix.m[1][0] = -s;
	rotationMatrix.m[1][1] = c;
	rotationMatrix.m[1][2] = 0.0f;
	rotationMatrix.m[1][3] = 0.0f;
	rotationMatrix.m[2][0] = 0.0f;
	rotationMatrix.m[2][1] = 0.0f;
	rotationMatrix.m[2][2] = 1.0f;
	rotationMatrix.m[2][3] = 0.0f;
	rotationMatrix.m[3][0] = 0.0f;
	rotationMatrix.m[3][1] = 0.0f;
	rotationMatrix.m[3][2] = 0.0f;
	rotationMatrix.m[3][3] = 1.0f;

	return rotationMatrix;
}

float Length(const Vector3& v) {
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector3 Normalize(const Vector3& v) {
	float length = Length(v);
	if (length == 0) {
		// ’·‚³‚ª0‚Ìê‡A³‹K‰»‚Å‚«‚È‚¢‚Ì‚ÅŒ³‚ÌƒxƒNƒgƒ‹‚ğ•Ô‚·‚©AƒGƒ‰[‚ğˆ—‚·‚é
		return v;
	}
	return { v.x / length, v.y / length, v.z / length };
}
