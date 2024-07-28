#include <Novice.h>
#include "MyMath.h"
#include "ImGui.h"
#include <algorithm>

struct Sphere {
	Vector3 center;
	float radius;
	unsigned int color;
};

struct AABB {
	Vector3 min;
	Vector3 max;
};


void DrawSphere(const Sphere* sphere, Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, int color)
{
	const uint32_t kSubdivisiont = 30;  // 分割数

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

			a = { sphere->radius * cosf(lon) * cosf(lat) + sphere->center.x,
				sphere->radius * sinf(lon) + sphere->center.y,
				sphere->radius * cosf(lon) * sinf(lat) + sphere->center.z };

			b = { sphere->radius * cosf(lon + kLonEvery) * cosf(lat) + sphere->center.x,
				sphere->radius * sinf(lon + kLonEvery) + sphere->center.y,
				sphere->radius * cosf(lon + kLonEvery) * sinf(lat) + sphere->center.z };

			c = { sphere->radius * cosf(lon) * cosf(lat + kLatEvery) + sphere->center.x
				,sphere->radius * sinf(lon) + sphere->center.y
				,sphere->radius * cosf(lon) * sinf(lat + kLatEvery) + sphere->center.z };

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

void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	// AABBの8つの頂点を計算
	Vector3 vertices[8] = {
		{aabb.min.x, aabb.min.y, aabb.min.z},
		{aabb.max.x, aabb.min.y, aabb.min.z},
		{aabb.max.x, aabb.max.y, aabb.min.z},
		{aabb.min.x, aabb.max.y, aabb.min.z},
		{aabb.min.x, aabb.min.y, aabb.max.z},
		{aabb.max.x, aabb.min.y, aabb.max.z},
		{aabb.max.x, aabb.max.y, aabb.max.z},
		{aabb.min.x, aabb.max.y, aabb.max.z}
	};

	// 画面座標に変換
	for (int i = 0; i < 8; ++i) {
		vertices[i] = Transform(Transform(vertices[i], viewProjectionMatrix), viewportMatrix);
	}

	// 辺を描画
	Novice::DrawLine(int(vertices[0].x), int(vertices[0].y), int(vertices[1].x), int(vertices[1].y), color);
	Novice::DrawLine(int(vertices[1].x), int(vertices[1].y), int(vertices[2].x), int(vertices[2].y), color);
	Novice::DrawLine(int(vertices[2].x), int(vertices[2].y), int(vertices[3].x), int(vertices[3].y), color);
	Novice::DrawLine(int(vertices[3].x), int(vertices[3].y), int(vertices[0].x), int(vertices[0].y), color);

	Novice::DrawLine(int(vertices[4].x), int(vertices[4].y), int(vertices[5].x), int(vertices[5].y), color);
	Novice::DrawLine(int(vertices[5].x), int(vertices[5].y), int(vertices[6].x), int(vertices[6].y), color);
	Novice::DrawLine(int(vertices[6].x), int(vertices[6].y), int(vertices[7].x), int(vertices[7].y), color);
	Novice::DrawLine(int(vertices[7].x), int(vertices[7].y), int(vertices[4].x), int(vertices[4].y), color);

	Novice::DrawLine(int(vertices[0].x), int(vertices[0].y), int(vertices[4].x), int(vertices[4].y), color);
	Novice::DrawLine(int(vertices[1].x), int(vertices[1].y), int(vertices[5].x), int(vertices[5].y), color);
	Novice::DrawLine(int(vertices[2].x), int(vertices[2].y), int(vertices[6].x), int(vertices[6].y), color);
	Novice::DrawLine(int(vertices[3].x), int(vertices[3].y), int(vertices[7].x), int(vertices[7].y), color);
}

bool IsCollision(const AABB& aabb, const Sphere& sphere) {
	using std::max;
	using std::min;

	// AABBの最も近い点を求める
	Vector3 closestPoint = {
		max(aabb.min.x, min(sphere.center.x, aabb.max.x)),
		max(aabb.min.y, min(sphere.center.y, aabb.max.y)),
		max(aabb.min.z, min(sphere.center.z, aabb.max.z))
	};

	// 球の中心と最も近い点との距離の二乗を計算
	Vector3 diff = closestPoint - sphere.center;
	float distanceSquared = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

	// 距離の二乗が球の半径の二乗以下であれば衝突
	if (distanceSquared <= (sphere.radius * sphere.radius)) {
		return true;
	} else {
		return false;
	}
}

const char kWindowTitle[] = "LE2C_08_オカムラ_タクマ_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Sphere sphere{
	{0.5f, 0.0f, 0.0f}, // AABBの右側面に接触
	1.0f,
	WHITE
	};

	AABB aabb1{
		.min{-0.5f, -0.5f, -0.5f},
		.max{0.5f, 0.5f, 0.5f},
	};
	unsigned int color1 = WHITE;

	Vector3 cameraTranslate{ 0.0f,1.9f,-6.49f };
	Vector3 cameraRotate{ 0.26f,0.0f,0.0f };


	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		ImGui::Begin("AABB Controls");
		ImGui::Text("AABB 1");
		ImGui::DragFloat3("AABB 1 Min", &aabb1.min.x, 0.01f);
		ImGui::DragFloat3("AABB 1 Max", &aabb1.max.x, 0.01f);
		ImGui::End();

		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);

		if (IsCollision(aabb1, sphere)) {
			color1 = BLUE;
		} else {
			color1 = WHITE;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(viewProjectionMatrix, viewportMatrix);
		DrawSphere(&sphere, viewProjectionMatrix, viewportMatrix, sphere.color);
		DrawAABB(aabb1, viewProjectionMatrix, viewportMatrix, color1);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
