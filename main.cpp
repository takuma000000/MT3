#include <Novice.h>
#include "MyMath.h"
#include "ImGui.h"
#include "MT3.h"

const char kWindowTitle[] = "LE2C_08_オカムラ_タクマ_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, 1280, 720);

    // キー入力結果を受け取る箱
    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    Vector3 translates[3] = {
        {0.2f, 1.0f, 0.0f},  // 肩
        {0.4f, 0.0f, 0.0f},  // 肘
        {0.3f, 0.0f, 0.0f}   // 手
    };
    Vector3 rotates[3] = {
        {0.0f, 0.0f, -6.8f}, // 肩
        {0.0f, 0.0f, -1.4f}, // 肘
        {0.0f, 0.0f, 0.0f}   // 手
    };
    Vector3 scales[3] = {
        {1.0f, 1.0f, 1.0f},  // 肩
        {1.0f, 1.0f, 1.0f},  // 肘
        {1.0f, 1.0f, 1.0f}   // 手
    };

    Vector3 cameraTranslate{ 0.0f, 1.9f, -6.49f };
    Vector3 cameraRotate{ 0.26f, 0.0f, 0.0f };

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
        ImGui::Begin("Parameters");
        ImGui::DragFloat3("cameraTranslate", &cameraTranslate.x, 0.01f);
        ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
        ImGui::DragFloat3("Shoulder Translate", &translates[0].x, 0.01f);
        ImGui::DragFloat3("Shoulder Rotate", &rotates[0].x, 0.01f);
        ImGui::DragFloat3("Elbow Translate", &translates[1].x, 0.01f);
        ImGui::DragFloat3("Elbow Rotate", &rotates[1].x, 0.01f);
        ImGui::DragFloat3("Hand Translate", &translates[2].x, 0.01f);
        ImGui::DragFloat3("Hand Rotate", &rotates[2].x, 0.01f);
        ImGui::End();

        Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, cameraTranslate);
        Matrix4x4 viewMatrix = Inverse(cameraMatrix);
        Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
        Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
        Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);

        // 肩のワールド行列を計算
        Matrix4x4 shoulderMatrix = MakeAffineMatrix(scales[0], rotates[0], translates[0]);

        // 肘のワールド行列を計算
        Matrix4x4 elbowMatrix = MakeAffineMatrix(scales[1], rotates[1], translates[1]);
        elbowMatrix = Multiply(shoulderMatrix, elbowMatrix);

        // 手のワールド行列を計算
        Matrix4x4 handMatrix = MakeAffineMatrix(scales[2], rotates[2], translates[2]);
        handMatrix = Multiply(elbowMatrix, handMatrix);

        // 座標変換
        Vector3 shoulderPos = Transform({ 0.0f, 0.0f, 0.0f }, shoulderMatrix);
        Vector3 elbowPos = Transform({ 0.0f, 0.0f, 0.0f }, elbowMatrix);
        Vector3 handPos = Transform({ 0.0f, 0.0f, 0.0f }, handMatrix);

        Sphere shoulderSphere = { shoulderPos, 1.0f, {0,0,0}, {0,0,0}, 0.1f, RED };
        Sphere elbowSphere = { elbowPos, 1.0f, {0,0,0}, {0,0,0}, 0.1f, GREEN };
        Sphere handSphere = { handPos, 1.0f, {0,0,0}, {0,0,0}, 0.1f, BLUE };

        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///
        DrawGrid(viewProjectionMatrix, viewportMatrix);

        // 肩、肘、手の位置に球体を描画
        DrawSphere(shoulderSphere, viewProjectionMatrix, viewportMatrix, RED);
        DrawSphere(elbowSphere, viewProjectionMatrix, viewportMatrix, GREEN);
        DrawSphere(handSphere, viewProjectionMatrix, viewportMatrix, BLUE);

        // 肩から肘、肘から手への線を描画
        DrawLine(shoulderPos, elbowPos, viewProjectionMatrix, viewportMatrix, WHITE);
        DrawLine(elbowPos, handPos, viewProjectionMatrix, viewportMatrix, WHITE);

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
