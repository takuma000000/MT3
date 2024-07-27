#include <Novice.h>
#include "MyMath.h"
#include "ImGui.h"

struct Sphere {
	Vector3 center;
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

const char kWindowTitle[] = "LE2C_08_オカムラ_タクマ_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	float angle = 0.0f;
	float angularVelocity = 3.14f; // 角速度
	float radius = 0.8f; // 円運動の半径

	Vector3 cameraTranslate{ 0.0f,1.9f,-6.49f };
	Vector3 cameraRotate{ 0.26f,0.0f,0.0f };

	Sphere sphere{
		{ 0.8f,0.0f,0.0f },
		0.05f,
		WHITE
	};

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

		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);

		// 角度を更新
		angle += angularVelocity * (1.0f / 60.0f);

		// 球体の位置を更新
		sphere.center.x = radius * cosf(angle);
		sphere.center.y = radius * sinf(angle);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(viewProjectionMatrix, viewportMatrix);
		DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, sphere.color);

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
