#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "maze.h"
#include "game.h"
#include "giris.h"

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
    // Normal (50 px) butonda 20; dokunmatikte buyuyen butonda yazi da buyur
    int boyut = (rec.height > 50) ? (int)(rec.height * 0.36f) : 20;
    // Buyuyen butonda uzun yazi ("TABLOYU SIFIRLA") tasmasin; normal butona dokunulmaz
    while (rec.height > 50 && boyut > 12 && MeasureText(text, boyut) > rec.width - 20) boyut--;
    int textW = MeasureText(text, boyut);
    DrawText(text, rec.x + (rec.width - textW) / 2, rec.y + (rec.height - boyut) / 2, boyut, BLACK);
    if (tiklama_birakildi(rec)) tiklandi = true;
    return tiklandi;
}

// Odaktaki yazi kutusunda (tarayici) yanip sonen imlec
void ImlecCiz(int x, int y, int boyut) {
    if (((int)(GetTime() * 2)) % 2 == 0) DrawRectangle(x + 2, y, 2, boyut, BLACK);
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
        giris_guncelle(); // fare + dokunma durumunu bu kare icin oku

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

            // Dokunmatikte butonlar buyur (h), bilgisayarda 200x50 ve ayni konumlar
            int h = dokunma_boyu(50);
            int bw = dokunmatik_mi() ? 340 : 200;
            int bx = (800 - bw) / 2;
            if (ButonCiz("OYUNU BASLAT", (Rectangle){bx, 250, bw, h})) durum = AYARLAR;
            if (ButonCiz("SKOR TABLOSU", (Rectangle){bx, 250 + (h + 20), bw, h})) {
                reset_search_ui();
                durum = SKORLAR;
            }
#ifndef __EMSCRIPTEN__
            // Tarayicida "cikis" sayfayi donuk birakir; sekmeyi kapatmak yeterli
            if (ButonCiz("CIKIS", (Rectangle){bx, 250 + 2 * (h + 20), bw, h})) break;
#endif
        }

        else if (durum == AYARLAR) {
            // Dokunmatikte buyuk labirentin hucreleri okunamayacak kadar kuculur: 20x20 sinir
            int en_fazla_w = dokunmatik_mi() ? 20 : 50;
            int en_fazla_h = dokunmatik_mi() ? 20 : 25;

            // Bilgisayarda eski konumlar; dokunmatikte kutular ve butonlar parmak boyunda
            bool d = dokunmatik_mi();
            int h = dokunma_boyu(50);   // buton yuksekligi
            int kh = dokunma_boyu(30);  // yazi kutusu yuksekligi
            int etiket = d ? 26 : 20;   // etiket yazi boyutu
            int kyazi = d ? (int)(kh * 0.5f) : 20; // kutu ici yazi boyutu
            Rectangle kutu1 = d ? (Rectangle){440, 100, 200, kh} : (Rectangle){400, 195, 100, 30};
            Rectangle kutu2 = d ? (Rectangle){440, 100 + kh + 20, 200, kh} : (Rectangle){400, 245, 100, 30};
            Rectangle btnBaslat = d ? (Rectangle){150, kutu2.y + kh + 40, 240, h} : (Rectangle){300, 350, 200, 50};
            Rectangle btnGeri = d ? (Rectangle){410, kutu2.y + kh + 40, 240, h} : (Rectangle){300, 420, 200, 50};

            if (d) {
                const char *baslik = "LABIRENT BOYUTUNU GIRIN";
                DrawText(baslik, (800 - MeasureText(baslik, 30)) / 2, 40, 30, DARKGRAY);
            } else {
                DrawText("LABIRENT BOYUTUNU GIRIN", 200, 100, 30, DARKGRAY);
            }

            if (tiklama_basildi(kutu1)) aktif_kutu = 0;
            if (tiklama_basildi(kutu2)) aktif_kutu = 1;

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

            // Tarayici: kutularin ustundeki <input>'larla esitle; dokunulan kutu aktif olur
            bool odak1 = yazi_kutusu(KUTU_GENISLIK, kutu1, genislik_str, 2);
            bool odak2 = yazi_kutusu(KUTU_YUKSEKLIK, kutu2, yukseklik_str, 2);
            if (odak1) aktif_kutu = 0;
            if (odak2) aktif_kutu = 1;

            const char *etiket1 = TextFormat("Genislik (5-%d):", en_fazla_w);
            DrawText(etiket1, d ? 160 : 200, d ? kutu1.y + (kh - etiket) / 2 : 200, etiket, BLACK);
            DrawRectangleRec(kutu1, (aktif_kutu == 0) ? SKYBLUE : LIGHTGRAY);
            int yazi_y1 = d ? kutu1.y + (kh - kyazi) / 2 : 200;
            DrawText(genislik_str, kutu1.x + 10, yazi_y1, kyazi, BLACK);
            if (odak1) ImlecCiz(kutu1.x + 10 + MeasureText(genislik_str, kyazi), yazi_y1, kyazi);

            const char *etiket2 = TextFormat("Yukseklik (5-%d):", en_fazla_h);
            DrawText(etiket2, d ? 160 : 200, d ? kutu2.y + (kh - etiket) / 2 : 250, etiket, BLACK);
            DrawRectangleRec(kutu2, (aktif_kutu == 1) ? SKYBLUE : LIGHTGRAY);
            int yazi_y2 = d ? kutu2.y + (kh - kyazi) / 2 : 250;
            DrawText(yukseklik_str, kutu2.x + 10, yazi_y2, kyazi, BLACK);
            if (odak2) ImlecCiz(kutu2.x + 10 + MeasureText(yukseklik_str, kyazi), yazi_y2, kyazi);

            if (ButonCiz("BASLAT", btnBaslat)) {
                int w = atoi(genislik_str);
                int y_boyut = atoi(yukseklik_str);
                if (w < 5) w = 5; if (w > en_fazla_w) w = en_fazla_w;
                if (y_boyut < 5) y_boyut = 5; if (y_boyut > en_fazla_h) y_boyut = en_fazla_h;

#ifdef __EMSCRIPTEN__
                EndDrawing(); // tarayicida pencere kapanmadigi icin acik kalan cizimi bitir
#else
                CloseWindow();
#endif
                Labirent oyun;
                labirenti_hazirla(&oyun, w, y_boyut);
                labirenti_uret(&oyun);
                son_puan = oyunu_baslat(&oyun);
                labirenti_temizle(&oyun);

#ifdef __EMSCRIPTEN__
                BeginDrawing(); // dongunun sonundaki EndDrawing ile eslessin
#else
                InitWindow(800, 600, "Labirent");
#endif
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
            if (ButonCiz("GERI", btnGeri)) durum = MENU;
        }

        else if (durum == ISIM_GIRIS) {
            bool d = dokunmatik_mi();
            int h = dokunma_boyu(50);
            int kh = dokunma_boyu(40);
            int kyazi = d ? (int)(kh * 0.45f) : 30;
            Rectangle kutu = d ? (Rectangle){150, 180, 500, kh} : (Rectangle){250, 280, 300, 40};
            Rectangle btnKaydet = d ? (Rectangle){250, kutu.y + kh + 30, 300, h} : (Rectangle){300, 350, 200, 50};

            DrawText("TEBRIKLER!", 300, d ? 30 : 100, 40, GREEN);
            DrawText(TextFormat("PUANINIZ: %d", son_puan), 300, d ? 80 : 150, 30, DARKBLUE);
            DrawText("Lutfen isminizi giriniz:", d ? 150 : 250, d ? 140 : 250, d ? 26 : 20, BLACK);

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

            // Tarayici: kutunun ustundeki <input> ile esitle (telefonda klavye buradan acilir)
            bool odak = yazi_kutusu(KUTU_ISIM, kutu, oyuncu_ismi, 15);
            isim_harf_sayisi = (int)strlen(oyuncu_ismi);

            DrawRectangleRec(kutu, LIGHTGRAY);
            DrawRectangleLines(kutu.x, kutu.y, kutu.width, kutu.height, DARKGRAY);
            int yazi_y = d ? kutu.y + (kh - kyazi) / 2 : kutu.y + 10;
            DrawText(oyuncu_ismi, kutu.x + 10, yazi_y, kyazi, BLACK);
            if (odak) ImlecCiz(kutu.x + 10 + MeasureText(oyuncu_ismi, kyazi), yazi_y, kyazi);

            if (ButonCiz("KAYDET", btnKaydet)) {
                if (isim_harf_sayisi > 0) {
                    add_score(oyuncu_ismi, son_puan);
                    reset_search_ui(); // <--- YENI: Kaydettikten sonra da arama temizlensin
                    durum = SKORLAR;
                }
            }
        }

        else if (durum == SKORLAR) {
            show_scores_gui();
            // Dokunmatikte en altta yan yana iki buyuk buton (maze.c'deki duzenle uyumlu)
            int h = dokunma_boyu(50);
            bool d = dokunmatik_mi();
            Rectangle btnDon = d ? (Rectangle){100, 590 - h, 290, h} : (Rectangle){100, 500, 200, 50};
            Rectangle btnSifirla = d ? (Rectangle){410, 590 - h, 290, h} : (Rectangle){500, 500, 200, 50};
            if (ButonCiz("MENUYE DON", btnDon)) durum = MENU;
            if (ButonCiz("TABLOYU SIFIRLA", btnSifirla)) reset_scores();
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}