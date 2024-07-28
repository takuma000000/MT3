#include <Novice.h>
#include "MyMath.h"
#include "ImGui.h"

struct Segment {
	Vector3 origin;
	Vector3 diff;
	unsigned int color;
};

struct Triangle {
	Vector3 ver[3];
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

void DrawLine(const Vector3& p1, const Vector3& p2, const Matrix4x4& wvpMatrix, const Matrix4x4& viewportMatrix, int32_t color) {

	Vector3 startPos = Transform(p1, wvpMatrix);
	Vector3 endPos = Transform(p2, wvpMatrix);

	startPos = Transform(startPos, viewportMatrix);
	endPos = Transform(endPos, viewportMatrix);

	Novice::DrawLine(int(startPos.x), int(startPos.y), int(endPos.x), int(endPos.y), color);
}

bool IsCollsion(const Triangle& triangle, const Segment& segment) {
	const float EPSILON = 1e-5f;

	// 三角形の辺ベクトルを求める
	Vector3 edge1 = triangle.ver[1] - triangle.ver[0];
	Vector3 edge2 = triangle.ver[2] - triangle.ver[0];

	// 平面の法線ベクトルを求める
	Vector3 normal = Cross(edge1, edge2);
	float normalDotRayDirection = Dot(normal, segment.diff);

	// 線分と平面が平行かチェック
	if (std::fabs(normalDotRayDirection) < EPSILON) {
		return false; // 平行なので交差しない
	}

	// 平面と線分の交差点を求める
	float d = Dot(normal, triangle.ver[0]);
	float t = (d - Dot(normal, segment.origin)) / normalDotRayDirection;

	// tが0から1の範囲外なら交差していない
	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	// 交差点の座標を求める
	Vector3 intersection = segment.origin + segment.diff * t;

	// 交差点が三角形の内部にあるかチェックする
	Vector3 edge;
	Vector3 C;

	// 頂点0、1と交差点のベクトルを計算
	edge = triangle.ver[1] - triangle.ver[0];
	C = intersection - triangle.ver[0];
	if (Dot(normal, Cross(edge, C)) < 0) {
		return false;
	}

	// 頂点1、2と交差点のベクトルを計算
	edge = triangle.ver[2] - triangle.ver[1];
	C = intersection - triangle.ver[1];
	if (Dot(normal, Cross(edge, C)) < 0) {
		return false;
	}

	// 頂点2、0と交差点のベクトルを計算
	edge = triangle.ver[0] - triangle.ver[2];
	C = intersection - triangle.ver[2];
	if (Dot(normal, Cross(edge, C)) < 0) {
		return false;
	}

	return true; // 交差している
}

void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	// 各頂点を変換する
	Vector3 screenVertices[3];
	for (int i = 0; i < 3; ++i) {
		screenVertices[i] = Transform(triangle.ver[i], viewProjectionMatrix);
		screenVertices[i] = Transform(screenVertices[i], viewportMatrix);
	}

	// 三角形を描画する
	Novice::DrawLine(int(screenVertices[0].x), int(screenVertices[0].y), int(screenVertices[1].x), int(screenVertices[1].y), color);
	Novice::DrawLine(int(screenVertices[1].x), int(screenVertices[1].y), int(screenVertices[2].x), int(screenVertices[2].y), color);
	Novice::DrawLine(int(screenVertices[2].x), int(screenVertices[2].y), int(screenVertices[0].x), int(screenVertices[0].y), color);
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

	Segment segment{
		{0.0f,1.7f,0.0f},
		{0.0f,-1.4f,0.0f},
		WHITE
	};

	Triangle triangle{
	{ {0.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f} },
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

		ImGui::Begin("Segment Controls");
		ImGui::DragFloat3("Origin", &segment.origin.x, 0.1f);
		ImGui::DragFloat3("Diff", &segment.diff.x, 0.1f);
		ImGui::End();

		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);

		if (IsCollsion(triangle, segment)) {
			segment.color = BLUE;
		} else {
			segment.color = WHITE;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(viewProjectionMatrix, viewportMatrix);
		DrawLine(segment.origin, segment.diff, viewProjectionMatrix, viewportMatrix, segment.color);
		DrawTriangle(triangle, viewProjectionMatrix, viewportMatrix, triangle.color);

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
