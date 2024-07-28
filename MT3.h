#pragma once
#include "Novice.h"
#include "MyMath.h"

struct Sphere {
	Vector3 center;
	float mass;
	Vector3 acc;
	Vector3 vel;
	float radius;
	unsigned int color;
};

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);

	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHalfWidth + (xIndex * kGridEvery);
		unsigned int color = 0xAAAAAAFF;
		Vector3 start{ x,0.0f,-kGridHalfWidth };
		Vector3 end{ x,0.0f,kGridHalfWidth };

		Vector3 startScreen = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 endScreen = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		if (x == 0.0f) {
			color = BLACK;
		}

		Novice::DrawLine(int(startScreen.x), int(startScreen.y), int(endScreen.x), int(endScreen.y), color);
	}

	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHalfWidth + (zIndex * kGridEvery);
		unsigned int color = 0xAAAAAAFF;

		Vector3 start{ -kGridHalfWidth,0.0f,z };
		Vector3 end{ kGridHalfWidth,0.0f, z };

		Vector3 startScreen = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 endScreen = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		if (z == 0.0f) {
			color = BLACK;
		}

		Novice::DrawLine(int(startScreen.x), int(startScreen.y), int(endScreen.x), int(endScreen.y), color);
	}

}

void DrawSphere(const Sphere& sphere, Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, int color)
{
	const uint32_t kSubdivisiont = 10;  // 分割数

	const float kLonEvery = 2.0f * float(M_PI) / kSubdivisiont;  // 経度分割1つ分の角度
	const float kLatEvery = 2.0f * float(M_PI) / kSubdivisiont;  // 緯度分割1つ分の角度

	// 緯度の方向に分割　-π/2 ~ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivisiont; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex; // 現在の軽度

		// 経度の方向に分割0 ~ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivisiont; ++lonIndex)
		{
			float lon = lonIndex * kLonEvery; // 現在の軽度

			Vector3 a, b, c;

			a = { sphere.radius * cosf(lon) * cosf(lat) + sphere.center.x,
				sphere.radius * sinf(lon) + sphere.center.y,
				sphere.radius * cosf(lon) * sinf(lat) + sphere.center.z };

			b = { sphere.radius * cosf(lon + kLonEvery) * cosf(lat) + sphere.center.x,
				sphere.radius * sinf(lon + kLonEvery) + sphere.center.y,
				sphere.radius * cosf(lon + kLonEvery) * sinf(lat) + sphere.center.z };

			c = { sphere.radius * cosf(lon) * cosf(lat + kLatEvery) + sphere.center.x
				,sphere.radius * sinf(lon) + sphere.center.y
				,sphere.radius * cosf(lon) * sinf(lat + kLatEvery) + sphere.center.z };

			// ワールド座標系
			Matrix4x4 WorldMatrixa = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, a);
			Matrix4x4 WorldMatrixb = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, b);
			Matrix4x4 WorldMatrixc = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, c);

			// 同時クリップ空間
			Matrix4x4 WorldViewProjectionMatrixa = Multiply(WorldMatrixa, viewProjectionMatrix);
			Matrix4x4 WorldViewProjectionMatrixb = Multiply(WorldMatrixb, viewProjectionMatrix);
			Matrix4x4 WorldViewProjectionMatrixc = Multiply(WorldMatrixc, viewProjectionMatrix);

			// 正規化デバイス座標系
			Vector3 ndcVertexa = Transform({ 0,0,0 }, WorldViewProjectionMatrixa);
			Vector3 ndcVertexb = Transform({ 0,0,0 }, WorldViewProjectionMatrixb);
			Vector3 ndcVertexc = Transform({ 0,0,0 }, WorldViewProjectionMatrixc);

			// スクリーン座標系
			Vector3 screenVertexa = Transform(ndcVertexa, viewportMatrix);
			Vector3 screenVertexb = Transform(ndcVertexb, viewportMatrix);
			Vector3 screenVertexc = Transform(ndcVertexc, viewportMatrix);

			// ab,ac で線を引く
			Novice::DrawLine(int(screenVertexa.x), int(screenVertexa.y), int(screenVertexb.x), int(screenVertexb.y), color);
			Novice::DrawLine(int(screenVertexa.x), int(screenVertexa.y), int(screenVertexc.x), int(screenVertexc.y), color);
		}
	}
}

void DrawLine(const Vector3& p1, const Vector3& p2, const Matrix4x4& wvpMatrix, const Matrix4x4& viewportMatrix, int32_t color) {

	Vector3 startPos = Transform(p1, wvpMatrix);
	Vector3 endPos = Transform(p2, wvpMatrix);

	startPos = Transform(startPos, viewportMatrix);
	endPos = Transform(endPos, viewportMatrix);

	Novice::DrawLine(int(startPos.x), int(startPos.y), int(endPos.x), int(endPos.y), color);
}
