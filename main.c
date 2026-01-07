#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "maze.h"
#include "game.h"

typedef enum {
    MENU,
    AYARLAR,
    OYUN,
    ISIM_GIRIS,
    SKORLAR
} OyunDurumu;

bool ButonCiz(const char *text, Rectangle rec) {
    bool tiklandi = false;
    Vector2 mouse = GetMousePosition();
    bool uzerinde = CheckCollisionPointRec(mouse, rec);
    Color renk = uzerinde ? SKYBLUE : LIGHTGRAY;
    if (uzerinde && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) renk = BLUE;
    DrawRectangleRec(rec, renk);
    DrawRectangleLinesEx(rec, 2, DARKGRAY);
    int textW = MeasureText(text, 20);
    DrawText(text, rec.x + (rec.width - textW) / 2, rec.y + (rec.height - 20) / 2, 20, BLACK);
    if (uzerinde && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) tiklandi = true;
    return tiklandi;
}

int main() {
    srand(time(NULL));

    InitWindow(800, 600, "Labirent");
    SetTargetFPS(60);

    Image icon = GenImageColor(64, 64, DARKGRAY);
    ImageDrawRectangle(&icon, 8, 8, 48, 48, LIGHTGRAY);
    ImageDrawRectangle(&icon, 20, 20, 24, 24, DARKGRAY);
    ImageDrawRectangle(&icon, 24, 24, 16, 16, GREEN);
    SetWindowIcon(icon);
    UnloadImage(icon);

    OyunDurumu durum = MENU;

    char genislik_str[10] = "";
    char yukseklik_str[10] = "";
    char oyuncu_ismi[50] = "";
    int isim_harf_sayisi = 0;
    int aktif_kutu = 0;
    int son_puan = 0;

    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (durum == MENU) {
            // --- GUNCELLEME BASLANGICI ---
            const char* baslik = "LABIRENT";
            int yazi_boyutu = 50;
            int yazi_genisligi = MeasureText(baslik, yazi_boyutu); // Yazinin genisligini olc
            int ekran_genisligi = GetScreenWidth(); // 800

            // Tam ortaya koymak icin: (Ekran - Yazi) / 2
            int x_pos = (ekran_genisligi - yazi_genisligi) / 2;

            DrawText(baslik, x_pos, 100, yazi_boyutu, DARKBLUE);
            // --- GUNCELLEME BITISI ---


            // Butonlari da ayni mantikla ortalamak istersen soyle, onlari da formule dokelim.
            // Ama simdilik sadece baslik icin bu yeterli.

            if (ButonCiz("OYUNU BASLAT", (Rectangle){300, 250, 200, 50})) durum = AYARLAR;
            if (ButonCiz("SKOR TABLOSU", (Rectangle){300, 320, 200, 50})) {
                reset_search_ui();
                durum = SKORLAR;
            }
            if (ButonCiz("CIKIS", (Rectangle){300, 390, 200, 50})) break;
        }

        else if (durum == AYARLAR) {
            DrawText("LABIRENT BOYUTUNU GIRIN", 200, 100, 30, DARKGRAY);

            DrawText("Genislik (5-50):", 200, 200, 20, BLACK);
            Rectangle kutu1 = {400, 195, 100, 30};
            DrawRectangleRec(kutu1, (aktif_kutu == 0) ? SKYBLUE : LIGHTGRAY);
            DrawText(genislik_str, 410, 200, 20, BLACK);
            if (CheckCollisionPointRec(GetMousePosition(), kutu1) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) aktif_kutu = 0;

            DrawText("Yukseklik (5-25):", 200, 250, 20, BLACK);
            Rectangle kutu2 = {400, 245, 100, 30};
            DrawRectangleRec(kutu2, (aktif_kutu == 1) ? SKYBLUE : LIGHTGRAY);
            DrawText(yukseklik_str, 410, 250, 20, BLACK);
            if (CheckCollisionPointRec(GetMousePosition(), kutu2) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) aktif_kutu = 1;

            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 48 && key <= 57)) {
                    if (aktif_kutu == 0 && strlen(genislik_str) < 2) {
                        int len = strlen(genislik_str);
                        genislik_str[len] = (char)key;
                        genislik_str[len+1] = '\0';
                    } else if (aktif_kutu == 1 && strlen(yukseklik_str) < 2) {
                        int len = strlen(yukseklik_str);
                        yukseklik_str[len] = (char)key;
                        yukseklik_str[len+1] = '\0';
                    }
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
                if (aktif_kutu == 0 && strlen(genislik_str) > 0) genislik_str[strlen(genislik_str)-1] = '\0';
                if (aktif_kutu == 1 && strlen(yukseklik_str) > 0) yukseklik_str[strlen(yukseklik_str)-1] = '\0';
            }

            if (ButonCiz("BASLAT", (Rectangle){300, 350, 200, 50})) {
                int w = atoi(genislik_str);
                int h = atoi(yukseklik_str);
                if (w < 5) w = 5; if (w > 50) w = 50;
                if (h < 5) h = 5; if (h > 25) h = 25;

                CloseWindow();
                Labirent oyun;
                labirenti_hazirla(&oyun, w, h);
                labirenti_uret(&oyun);
                son_puan = oyunu_baslat(&oyun);
                labirenti_temizle(&oyun);

                InitWindow(800, 600, "Labirent");
                SetTargetFPS(60);
                Image icon2 = GenImageColor(64, 64, DARKGRAY);
                ImageDrawRectangle(&icon2, 8, 8, 48, 48, LIGHTGRAY);
                ImageDrawRectangle(&icon2, 20, 20, 24, 24, DARKGRAY);
                ImageDrawRectangle(&icon2, 24, 24, 16, 16, GREEN);
                SetWindowIcon(icon2);
                UnloadImage(icon2);

                if (son_puan > 0) {
                    durum = ISIM_GIRIS;
                    oyuncu_ismi[0] = '\0';
                    isim_harf_sayisi = 0;
                } else {
                    durum = MENU;
                }
            }
            if (ButonCiz("GERI", (Rectangle){300, 420, 200, 50})) durum = MENU;
        }

        else if (durum == ISIM_GIRIS) {
            DrawText("TEBRIKLER!", 300, 100, 40, GREEN);
            DrawText(TextFormat("PUANINIZ: %d", son_puan), 300, 150, 30, DARKBLUE);
            DrawText("Lutfen isminizi giriniz:", 250, 250, 20, BLACK);
            DrawRectangle(250, 280, 300, 40, LIGHTGRAY);
            DrawRectangleLines(250, 280, 300, 40, DARKGRAY);
            DrawText(oyuncu_ismi, 260, 290, 30, BLACK);

            int key = GetCharPressed();
            while (key > 0) {
                if (key == 32) key = 95; // Bosluk girilirse 95(_) yapalim (Input sirasinda da kontrol)
                                         // Ama maze.c'deki add_score zaten yapiyor, burada gorsel olarak
                                         // bosluk gormek istiyorsan 32 birakabilirsin.
                                         // Yukarida "bosluk gozuksun" demistin, o zaman:
                if (key == 95) key = 32; // Kullanici _ basarsa bosluk yapalim

                if (((key >= 32) && (key <= 126)) && (isim_harf_sayisi < 15)) {
                    oyuncu_ismi[isim_harf_sayisi] = (char)key;
                    oyuncu_ismi[isim_harf_sayisi+1] = '\0';
                    isim_harf_sayisi++;
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE)) {
                isim_harf_sayisi--;
                if (isim_harf_sayisi < 0) isim_harf_sayisi = 0;
                oyuncu_ismi[isim_harf_sayisi] = '\0';
            }

            if (ButonCiz("KAYDET", (Rectangle){300, 350, 200, 50})) {
                if (isim_harf_sayisi > 0) {
                    add_score(oyuncu_ismi, son_puan);
                    reset_search_ui(); // <--- YENI: Kaydettikten sonra da arama temizlensin
                    durum = SKORLAR;
                }
            }
        }

        else if (durum == SKORLAR) {
            show_scores_gui();
            if (ButonCiz("MENUYE DON", (Rectangle){100, 500, 200, 50})) durum = MENU;
            if (ButonCiz("TABLOYU SIFIRLA", (Rectangle){500, 500, 200, 50})) reset_scores();
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}