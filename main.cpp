#include <Novice.h>
#include "MyMath.h"
#include "ImGui.h"

struct Spring {
	Vector3 anchor;
	float naturalLength;
	float stiffness;
	float damp;
};

struct Ball {
	Vector3 pos;
	Vector3 vel;
	Vector3 acc;
	float mass;
	float radius;
	unsigned int color;
};

struct Sphere {
	Vector3 center;
	float radius;
};

void DrawLine(const Vector3& p1, const Vector3& p2, const Matrix4x4& wvpMatrix, const Matrix4x4& viewportMatrix, int32_t color) {

	Vector3 startPos = Transform(p1, wvpMatrix);
	Vector3 endPos = Transform(p2, wvpMatrix);

	startPos = Transform(startPos, viewportMatrix);
	endPos = Transform(endPos, viewportMatrix);

	Novice::DrawLine(int(startPos.x), int(startPos.y), int(endPos.x), int(endPos.y), color);
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

void DrawSphere(const Sphere& sphere, Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, int color)
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
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Spring spring{};
	spring.anchor = { 0.0f,0.0f,0.0f };
	spring.naturalLength = 1.0f;
	spring.stiffness = 100.0f;
	spring.damp = 2.0f;

	Ball ball{};
	ball.pos = { 1.2f,0.0f,0.0f };
	ball.mass = 2.0f;
	ball.radius = 0.05f;
	ball.color = BLUE;

	Sphere sphere{
		{ball.pos.x,ball.pos.y,ball.pos.z},
		ball.radius
	};

	float Time = 1.0f / 60.0f;

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

		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);

		sphere.center = ball.pos;

		Vector3 diff = ball.pos - spring.anchor;
		float length = Length(diff);
		if (length != 0.0f) {
			Vector3 direction = Normalize(diff);
			Vector3 restPosition = spring.anchor + direction * spring.naturalLength;
			Vector3 dis = (ball.pos - restPosition) * length;
			Vector3 rest = dis *- spring.stiffness ;
			Vector3 dam = ball.vel * -spring.damp;
			Vector3 force = rest + dam;
			ball.acc = force / ball.mass;
		}

			ball.vel += ball.acc * Time;
			ball.pos += ball.vel * Time;

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(viewProjectionMatrix, viewportMatrix);
		DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, ball.color);
		DrawLine(spring.anchor, ball.pos, viewProjectionMatrix, viewportMatrix, WHITE);

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
