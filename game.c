#include <stdio.h>
#include "game.h"
#include "stack.h"

int oyunu_baslat(Labirent *labirent) {
    labirent->oyuncu_x = 0;
    labirent->oyuncu_y = 0;
    int adim_sayisi = 0;
    int puan = 0;

    Stack hamle_gecmisi;
    stack_olustur(&hamle_gecmisi);

    int hucre_boyutu = 30;

    int labirent_genislik = labirent->genislik * hucre_boyutu;
    int labirent_yukseklik = labirent->yukseklik * hucre_boyutu;

    int ekran_genislik = labirent_genislik;
    int ekran_yukseklik = labirent_yukseklik + 60;

    if (ekran_genislik < 800) ekran_genislik = 800;
    if (ekran_yukseklik < 660) ekran_yukseklik = 660;
    if (ekran_genislik > 1600) ekran_genislik = 1600;
    if (ekran_yukseklik > 900) ekran_yukseklik = 900;

    InitWindow(ekran_genislik, ekran_yukseklik, "Labirent");
    SetTargetFPS(60);

    Image icon = GenImageColor(64, 64, DARKGRAY);
    ImageDrawRectangle(&icon, 8, 8, 48, 48, LIGHTGRAY);
    ImageDrawRectangle(&icon, 20, 20, 24, 24, DARKGRAY);
    ImageDrawRectangle(&icon, 24, 24, 16, 16, GREEN);
    SetWindowIcon(icon);
    UnloadImage(icon);

    // --- ADIL PUANLAMA ICIN HAZIRLIK ---
    en_kisa_yolu_bul(labirent);
    int ideal_adim = labirent->en_kisa_yol_uzunlugu;

    // Okları temizle
    for(int y=0; y<labirent->yukseklik; y++) {
        for(int x=0; x<labirent->genislik; x++) {
            labirent->harita[y][x].yol_yonu = YON_YOK;
        }
    }

    while (!WindowShouldClose()) {

        int x = labirent->oyuncu_x;
        int y = labirent->oyuncu_y;

        bool tus_basildi = false;
        bool hareket_basarili = false;

        if (IsKeyPressed(KEY_W)) {
            tus_basildi = true;
            if (!labirent->harita[y][x].duvar_ust) {
                stack_ekle(&hamle_gecmisi, x, y); labirent->oyuncu_y--; hareket_basarili = true;
            }
        }
        else if (IsKeyPressed(KEY_S)) {
            tus_basildi = true;
            if (!labirent->harita[y][x].duvar_alt) {
                stack_ekle(&hamle_gecmisi, x, y); labirent->oyuncu_y++; hareket_basarili = true;
            }
        }
        else if (IsKeyPressed(KEY_A)) {
            tus_basildi = true;
            if (!labirent->harita[y][x].duvar_sol) {
                stack_ekle(&hamle_gecmisi, x, y); labirent->oyuncu_x--; hareket_basarili = true;
            }
        }
        else if (IsKeyPressed(KEY_D)) {
            tus_basildi = true;
            if (!labirent->harita[y][x].duvar_sag) {
                stack_ekle(&hamle_gecmisi, x, y); labirent->oyuncu_x++; hareket_basarili = true;
            }
        }
        else if (IsKeyPressed(KEY_H)) { en_kisa_yolu_bul(labirent); }
        else if (IsKeyPressed(KEY_Z)) {
            if (!stack_bos_mu(&hamle_gecmisi)) {
                Nokta eski_konum = stack_cikar(&hamle_gecmisi);
                labirent->oyuncu_x = eski_konum.x;
                labirent->oyuncu_y = eski_konum.y;
            }
        }

        if (tus_basildi) {
            adim_sayisi++;
        }

        // --- KAZANMA KONTROLU (BEKLEME EKRANI KALDIRILDI) ---
        if (labirent->oyuncu_x == labirent->cikis_x && labirent->oyuncu_y == labirent->cikis_y) {

            // Puan Hesapla
            if (adim_sayisi < 1) adim_sayisi = 1;
            double oran = (double)ideal_adim / (double)adim_sayisi;
            puan = (int)(oran * 1000);

            if (puan > 1000) puan = 1000;
            if (puan < 0) puan = 0;

            // BEKLEME DONGUSU YOK! DIREKT CIKIS.
            break;
        }

        BeginDrawing();
            labirenti_ciz(labirent);
            DrawText(TextFormat("ADIM: %d", adim_sayisi), 20, 20, 20, WHITE);
            const char* info_text = "IPUCU [H] | GERI AL [Z]";
            int info_w = MeasureText(info_text, 20);
            DrawText(info_text, ekran_genislik - info_w - 20, 20, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();
    return puan;
}