#include "DxLib.h"

// 定数定義
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CELL_SIZE = 100; // 1マスのサイズ
const int OFFSET_X = (SCREEN_WIDTH - CELL_SIZE * 3) / 2;
const int OFFSET_Y = (SCREEN_HEIGHT - CELL_SIZE * 3) / 2;

int board[3][3] = { 0 }; // 0:空, 1:○, 2:×
int turn = 1;            // 1:○の番, 2:×の番
bool isGameOver = false;
int winner = 0;          // 0:なし, 1:○, 2:×, 3:引き分け

// 勝利判定関数
void CheckWinner() {
    // 横と縦のチェック
    for (int i = 0; i < 3; i++) {
        if (board[i][0] != 0 && board[i][0] == board[i][1] && board[i][1] == board[i][2]) winner = board[i][0];
        if (board[0][i] != 0 && board[0][i] == board[1][i] && board[1][i] == board[2][i]) winner = board[0][i];
    }
    // 斜めのチェック
    if (board[0][0] != 0 && board[0][0] == board[1][1] && board[1][1] == board[2][2]) winner = board[0][0];
    if (board[0][2] != 0 && board[0][2] == board[1][1] && board[1][1] == board[2][0]) winner = board[0][2];

    if (winner != 0) {
        isGameOver = true;
        return;
    }

    // 引き分けチェック（空きマスがあるか）
    bool isFull = true;
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            if (board[y][x] == 0) isFull = false;
        }
    }
    if (isFull) {
        winner = 3;
        isGameOver = true;
    }
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    ChangeWindowMode(TRUE);
    if (DxLib_Init() == -1) return -1;
    SetDrawScreen(DX_SCREEN_BACK);

    int mouseX, mouseY;
    bool prevMouseLeft = false;

    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
        ClearDrawScreen();

        // --- 入力処理 ---
        int mouseInput = GetMouseInput();
        GetMousePoint(&mouseX, &mouseY);

        // 左クリックされた瞬間を検知
        if ((mouseInput & MOUSE_INPUT_LEFT) && !prevMouseLeft && !isGameOver) {
            int gridX = (mouseX - OFFSET_X) / CELL_SIZE;
            int gridY = (mouseY - OFFSET_Y) / CELL_SIZE;

            // 盤面内かつ空きマスなら配置
            if (gridX >= 0 && gridX < 3 && gridY >= 0 && gridY < 3) {
                if (board[gridY][gridX] == 0) {
                    board[gridY][gridX] = turn;
                    CheckWinner();
                    turn = (turn == 1) ? 2 : 1;
                }
            }
        }
        prevMouseLeft = (mouseInput & MOUSE_INPUT_LEFT);

        // --- 描画処理 ---
        // グリッドの描画
        for (int i = 0; i <= 3; i++) {
            DrawLine(OFFSET_X + i * CELL_SIZE, OFFSET_Y, OFFSET_X + i * CELL_SIZE, OFFSET_Y + 3 * CELL_SIZE, GetColor(255, 255, 255));
            DrawLine(OFFSET_X, OFFSET_Y + i * CELL_SIZE, OFFSET_X + 3 * CELL_SIZE, OFFSET_Y + i * CELL_SIZE, GetColor(255, 255, 255));
        }

        // ○と×の描画
        for (int y = 0; y < 3; y++) {
            for (int x = 0; x < 3; x++) {
                int centerX = OFFSET_X + x * CELL_SIZE + CELL_SIZE / 2;
                int centerY = OFFSET_Y + y * CELL_SIZE + CELL_SIZE / 2;
                if (board[y][x] == 1) {
                    DrawCircle(centerX, centerY, 40, GetColor(255, 100, 100), FALSE, 3);
                }
                else if (board[y][x] == 2) {
                    DrawLine(centerX - 35, centerY - 35, centerX + 35, centerY + 35, GetColor(100, 100, 255), 3);
                    DrawLine(centerX + 35, centerY - 35, centerX - 35, centerY + 35, GetColor(100, 100, 255), 3);
                }
            }
        }

        // UI情報の表示
        if (!isGameOver) {
            DrawFormatString(20, 20, GetColor(255, 255, 255), "Turn: %s", (turn == 1 ? "O" : "X"));
        }
        else {
            const char* msg = (winner == 3) ? "Draw Game!" : (winner == 1 ? "O Wins!" : "X Wins!");
            DrawFormatString(SCREEN_WIDTH / 2 - 40, 20, GetColor(255, 255, 0), msg);
            DrawString(SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT - 40, "Press ESC to Quit", GetColor(200, 200, 200));
        }

        ScreenFlip();
    }

    DxLib_End();
    return 0;
}
