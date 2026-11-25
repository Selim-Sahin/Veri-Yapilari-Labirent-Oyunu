#include <stdio.h>
#include "game.h"
#include "giris.h"
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

    // Dokunmatikte canvas labirent kadar: 800x660'lik bos kenar telefonda labirenti
    // gereksiz kucultuyordu. 360: ust cubuktaki ADIM yazisina yer.
    int en_az_w = dokunmatik_mi() ? 360 : 800;
    int en_az_h = dokunmatik_mi() ? labirent_yukseklik + 60 : 660;
    if (ekran_genislik < en_az_w) ekran_genislik = en_az_w;
    if (ekran_yukseklik < en_az_h) ekran_yukseklik = en_az_h;
    if (ekran_genislik > 1600) ekran_genislik = 1600;
    if (ekran_yukseklik > 900) ekran_yukseklik = 900;

#ifdef __EMSCRIPTEN__
    // Tarayicida tek bir canvas var: pencereyi kapatip acmak yerine boyutunu degistiriyoruz
    SetWindowSize(ekran_genislik, ekran_yukseklik);
#else
    InitWindow(ekran_genislik, ekran_yukseklik, "Labirent");
#endif
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

    // Tarayici: dokunmatik paneli ac, MENU butonu (bilgisayar tarayicisinda canvas'ta)
    oyun_modu(true);
#ifdef __EMSCRIPTEN__
    Rectangle btnMenu = {ekran_genislik - 170, 10, 150, 40};
#endif
    double menu_onay_bitis = 0; // MENU'ye ilk basistan sonra 2 sn icinde ikinci basis cikar

    while (!WindowShouldClose()) {
        giris_guncelle(); // fare + dokunma durumunu bu kare icin oku

        int x = labirent->oyuncu_x;
        int y = labirent->oyuncu_y;

        bool tus_basildi = false;
        bool hareket_basarili = false;

        // Klavye, ekran tuslari ve kaydirma ayni komutlara doner; asagidaki zincir eskiden
        // IsKeyPressed(KEY_W/S/A/D/H/Z) idi. Adim sayisi, geri alma yigini ve puan ayni kod.
        OyunKomutu komut = oyun_komutu();
#ifdef __EMSCRIPTEN__
        if (komut == KOMUT_YOK && !dokunmatik_mi() && tiklama_birakildi(btnMenu)) komut = KOMUT_MENU;
#endif

        if (komut == KOMUT_YUKARI) {
            tus_basildi = true;
            if (!labirent->harita[y][x].duvar_ust) {
                stack_ekle(&hamle_gecmisi, x, y); labirent->oyuncu_y--; hareket_basarili = true;
            }
        }
        else if (komut == KOMUT_ASAGI) {
            tus_basildi = true;
            if (!labirent->harita[y][x].duvar_alt) {
                stack_ekle(&hamle_gecmisi, x, y); labirent->oyuncu_y++; hareket_basarili = true;
            }
        }
        else if (komut == KOMUT_SOL) {
            tus_basildi = true;
            if (!labirent->harita[y][x].duvar_sol) {
                stack_ekle(&hamle_gecmisi, x, y); labirent->oyuncu_x--; hareket_basarili = true;
            }
        }
        else if (komut == KOMUT_SAG) {
            tus_basildi = true;
            if (!labirent->harita[y][x].duvar_sag) {
                stack_ekle(&hamle_gecmisi, x, y); labirent->oyuncu_x++; hareket_basarili = true;
            }
        }
        else if (komut == KOMUT_IPUCU) { en_kisa_yolu_bul(labirent); }
        else if (komut == KOMUT_GERI_AL) {
            if (!stack_bos_mu(&hamle_gecmisi)) {
                Nokta eski_konum = stack_cikar(&hamle_gecmisi);
                labirent->oyuncu_x = eski_konum.x;
                labirent->oyuncu_y = eski_konum.y;
            }
        }

        else if (komut == KOMUT_MENU) {
            if (GetTime() < menu_onay_bitis) { puan = 0; break; } // ikinci basis: menuye don
            menu_onay_bitis = GetTime() + 2.0;
        }
        bool menu_onayda = GetTime() < menu_onay_bitis;
        menu_onay_goster(menu_onayda);

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
            // Dokunmatikte ayni isi ekran tuslari goruyor, tus ipucu gizlenir
            if (!dokunmatik_mi()) {
                const char* info_text = "IPUCU [H] | GERI AL [Z]";
                int info_w = MeasureText(info_text, 20);
                int sag_bosluk = 20;
#ifdef __EMSCRIPTEN__
                // Bilgisayar tarayicisi: menuye donmek icin MENU butonu (exe'de pencere kapatilir)
                Color renk = menu_onayda ? RED : (CheckCollisionPointRec(GetMousePosition(), btnMenu) ? SKYBLUE : LIGHTGRAY);
                const char *etiket = menu_onayda ? "CIKILSIN MI?" : "MENU";
                DrawRectangleRec(btnMenu, renk);
                DrawText(etiket, btnMenu.x + (btnMenu.width - MeasureText(etiket, 20)) / 2, btnMenu.y + 10, 20,
                         menu_onayda ? WHITE : BLACK);
                sag_bosluk += btnMenu.width + 20;
#endif
                DrawText(info_text, ekran_genislik - info_w - sag_bosluk, 20, 20, LIGHTGRAY);
            }
        EndDrawing();
    }
    oyun_modu(false);

#ifdef __EMSCRIPTEN__
    SetWindowSize(800, 600); // menu boyutuna geri don
#else
    CloseWindow();
#endif
    return puan;
}