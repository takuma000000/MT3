#include <Novice.h>
#include "MyMath.h"
#include "ImGui.h"
#include <algorithm>

struct AABB {
	Vector3 min;
	Vector3 max;
};

struct Segment {
	Vector3 origin;
	Vector3 diff;
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

void DrawLine(const Vector3& p1, const Vector3& p2, const Matrix4x4& wvpMatrix, const Matrix4x4& viewportMatrix, int32_t color) {

	Vector3 startPos = Transform(p1, wvpMatrix);
	Vector3 endPos = Transform(p2, wvpMatrix);

	startPos = Transform(startPos, viewportMatrix);
	endPos = Transform(endPos, viewportMatrix);

	Novice::DrawLine(int(startPos.x), int(startPos.y), int(endPos.x), int(endPos.y), color);
}

bool IsCollision(const AABB& aabb, const Segment& segment) {
	// 定義域を計算
	float tMin = 0.0f;
	float tMax = 1.0f;

	// 各軸に対してチェック
	// x軸
	float d = segment.diff.x;
	if (fabs(d) < 1e-6) {  // 小さな値を使用
		if (segment.origin.x < aabb.min.x || segment.origin.x > aabb.max.x) {
			return false; // 原点がAABBの範囲外
		}
	} else {
		float ood = 1.0f / d;
		float t1 = (aabb.min.x - segment.origin.x) * ood;
		float t2 = (aabb.max.x - segment.origin.x) * ood;
		if (t1 > t2) std::swap(t1, t2);
		if (t1 > tMin) tMin = t1;
		if (t2 < tMax) tMax = t2;
		if (tMin > tMax) return false; // 交差しない
	}

	// y軸
	d = segment.diff.y;
	if (fabs(d) < 1e-6) {
		if (segment.origin.y < aabb.min.y || segment.origin.y > aabb.max.y) {
			return false;
		}
	} else {
		float ood = 1.0f / d;
		float t1 = (aabb.min.y - segment.origin.y) * ood;
		float t2 = (aabb.max.y - segment.origin.y) * ood;
		if (t1 > t2) std::swap(t1, t2);
		if (t1 > tMin) tMin = t1;
		if (t2 < tMax) tMax = t2;
		if (tMin > tMax) return false;
	}

	// z軸
	d = segment.diff.z;
	if (fabs(d) < 1e-6) {
		if (segment.origin.z < aabb.min.z || segment.origin.z > aabb.max.z) {
			return false;
		}
	} else {
		float ood = 1.0f / d;
		float t1 = (aabb.min.z - segment.origin.z) * ood;
		float t2 = (aabb.max.z - segment.origin.z) * ood;
		if (t1 > t2) std::swap(t1, t2);
		if (t1 > tMin) tMin = t1;
		if (t2 < tMax) tMax = t2;
		if (tMin > tMax) return false;
	}

	return true; // 交差する
}


const char kWindowTitle[] = "LE2C_08_オカムラ_タクマ_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	AABB aabb1{
		.min{-0.5f, -0.5f, -0.5f},
		.max{0.5f, 0.5f, 0.5f},
	};
	unsigned int color1 = WHITE;

	Segment segment{
		{-0.7f,0.3f,0.0f},
		{2.0f,-0.5f,0.0f},
		WHITE
	};


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
		ImGui::Text("Segment");
		ImGui::DragFloat3("Origin", &segment.origin.x, 0.1f);
		ImGui::DragFloat3("Diff", &segment.diff.x, 0.1f);
		ImGui::End();

		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);

		if (IsCollision(aabb1, segment)) {
			color1 = GREEN;
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
		DrawAABB(aabb1, viewProjectionMatrix, viewportMatrix, color1);
		DrawLine(segment.origin, segment.diff, viewProjectionMatrix, viewportMatrix, segment.color);

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
