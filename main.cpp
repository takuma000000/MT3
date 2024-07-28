#include <Novice.h>
#include "MyMath.h"
#include "ImGui.h"

struct AABB {
	Vector3 min;
	Vector3 max;
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

bool IsCollision(const AABB& aabb1, const AABB& aabb2) {
	// X軸方向の重なりをチェック
	if (aabb1.max.x < aabb2.min.x || aabb1.min.x > aabb2.max.x) return false;
	// Y軸方向の重なりをチェック
	if (aabb1.max.y < aabb2.min.y || aabb1.min.y > aabb2.max.y) return false;
	// Z軸方向の重なりをチェック
	if (aabb1.max.z < aabb2.min.z || aabb1.min.z > aabb2.max.z) return false;

	// 全ての軸で重なっている場合、衝突している
	return true;
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

const char kWindowTitle[] = "LE2C_08_オカムラ_タクマ_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Vector3 cameraTranslate{ 0.0f,1.9f,-6.49f };
	Vector3 cameraRotate{ 0.26f,0.0f,0.0f };

	AABB aabb1{
	.min{-0.5f, -0.5f, -0.5f},
	.max{0.5f, 0.5f, 0.5f},
	};

	AABB aabb2{
		.min{0.0f, 0.0f, 0.0f},
		.max{1.0f, 1.0f, 1.0f},
	};

	unsigned int color1 = WHITE;
	unsigned int color2 = WHITE;

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
		ImGui::Text("AABB 2");
		ImGui::DragFloat3("AABB 2 Min", &aabb2.min.x, 0.01f);
		ImGui::DragFloat3("AABB 2 Max", &aabb2.max.x, 0.01f);
		ImGui::End();

		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);

		// AABBの衝突判定
		if (IsCollision(aabb1, aabb2)) {
			color1 = RED;
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
		DrawAABB(aabb2, viewProjectionMatrix, viewportMatrix, color2);

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
