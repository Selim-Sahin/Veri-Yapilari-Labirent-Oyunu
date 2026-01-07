#ifndef LABIRENT_OYUNU_MAZE_H
#define LABIRENT_OYUNU_MAZE_H

#include <stdbool.h>
#include "raylib.h"

// YONLER
typedef enum {
    YON_YOK = 0,
    YON_YUKARI,
    YON_ASAGI,
    YON_SOL,
    YON_SAG
} Yon;

// HUCRE
typedef struct {
    int x, y;
    bool duvar_ust;
    bool duvar_alt;
    bool duvar_sol;
    bool duvar_sag;
    bool ziyaret_edildi;
    bool ozel_konum;
    Yon yol_yonu;
} Hucre;

// LABIRENT
typedef struct {
    int genislik;
    int yukseklik;
    Hucre **harita;
    int oyuncu_x;
    int oyuncu_y;
    int cikis_x;
    int cikis_y;
    int en_kisa_yol_uzunlugu;
} Labirent;

// AGAC YAPISI
typedef struct SkorDugumu {
    char isim[50];
    int puan;
    struct SkorDugumu *sol;
    struct SkorDugumu *sag;
} SkorDugumu;

// FONKSIYONLAR
void labirenti_hazirla(Labirent *labirent, int w, int h);
void labirenti_temizle(Labirent *labirent);
void labirenti_ciz(Labirent *labirent);
void labirenti_uret(Labirent *labirent);
void en_kisa_yolu_bul(Labirent *labirent);

// Skor Yonetimi
void add_score(char *name, int point);
void show_scores_gui();
void reset_scores();
void reset_search_ui(); // <--- YENI EKLEME: Arama temizleme fonksiyonu

// Algoritmalar
void quick_sort(SkorDugumu **arr, int low, int high);
int binary_search(SkorDugumu **arr, int l, int r, char *name);

#endif //LABIRENT_OYUNU_MAZE_H