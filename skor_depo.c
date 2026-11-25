#include "skor_depo.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static SkorKaydi liste[SKOR_EN_FAZLA];
static int liste_adet = 0;

void isim_duzenle(char *isim) {
    int n = 0;
    bool bosluk = false;
    for (int i = 0; isim[i] != '\0'; i++) {
        if (isim[i] == ' ') { bosluk = (n > 0); continue; }
        if (bosluk) { isim[n++] = ' '; bosluk = false; }
        isim[n++] = isim[i];
    }
    isim[n] = '\0';
}

const SkorKaydi *skor_listesi(int *adet) {
    *adet = liste_adet;
    return liste;
}

#ifdef __EMSCRIPTEN__
// --- Tarayici: Firestore (asil is web/shell.html'deki Module.skorKaydet / skorlariYukle) ---
#include <emscripten.h>

void skor_kaydet_baslat(const char *isim, int puan, int genislik, int yukseklik) {
    EM_ASM({ Module.skorKaydet(UTF8ToString($0), $1, $2, $3); }, isim, puan, genislik, yukseklik);
}

DepoDurumu skor_kaydet_durumu(void) {
    return (DepoDurumu)EM_ASM_INT({ return Module.kayitDurumu; });
}

static int alinan_surum = 0; // C'ye en son kopyalanan tablonun surumu

void skorlari_yukle_baslat(void) {
    EM_ASM({ Module.skorlariYukle(); });
}

DepoDurumu skorlari_yukle_durumu(void) {
    int durum = EM_ASM_INT({ return Module.tabloDurumu; });
    int surum = EM_ASM_INT({ return Module.tabloSurumu; });
    if (durum == DEPO_TAMAM && surum != alinan_surum) {
        // Yeni gelen tabloyu bir kez C dizisine kopyala; her karede JS'e gitmeyelim
        alinan_surum = surum;
        int n = EM_ASM_INT({ return Module.skorTablosu.length; });
        if (n > SKOR_EN_FAZLA) n = SKOR_EN_FAZLA;
        for (int i = 0; i < n; i++) {
            SkorKaydi *k = &liste[i];
            EM_ASM({ stringToUTF8(Module.skorTablosu[$0].isim, $1, $2); }, i, k->isim, (int)sizeof k->isim);
            k->puan = EM_ASM_INT({ return Module.skorTablosu[$0].puan; }, i);
            k->genislik = EM_ASM_INT({ return Module.skorTablosu[$0].genislik; }, i);
            k->yukseklik = EM_ASM_INT({ return Module.skorTablosu[$0].yukseklik; }, i);
        }
        liste_adet = n;
    }
    return (DepoDurumu)durum;
}

void skorlari_sifirla(void) {
    // Tarayicida bilerek bos: ortak tablo yalnizca Firebase konsolundan silinir.
}

#else
// --- Masaustu: yerel highscore.txt ---
// Satir bicimi: "isim puan genislik yukseklik" (isimdeki bosluklar '_' olarak yazilir).
// Eski surumun "isim puan" satirlari da okunur; boyutlari 0 (bilinmiyor) kalir.

#define SKOR_DOSYASI "highscore.txt"

static DepoDurumu kayit_durumu = DEPO_BOSTA;
static DepoDurumu yukleme_durumu = DEPO_BOSTA;

static bool ayni_isim(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return false;
        a++; b++;
    }
    return *a == *b;
}

static void dosyadan_oku(void) {
    liste_adet = 0;
    FILE *file = fopen(SKOR_DOSYASI, "r");
    if (file == NULL) return;
    char satir[128];
    while (liste_adet < SKOR_EN_FAZLA && fgets(satir, sizeof satir, file)) {
        SkorKaydi k = {0};
        int alan = sscanf(satir, "%49s %d %d %d", k.isim, &k.puan, &k.genislik, &k.yukseklik);
        if (alan < 2) continue;
        if (alan < 4) { k.genislik = 0; k.yukseklik = 0; }
        for (int i = 0; k.isim[i] != '\0'; i++) if (k.isim[i] == '_') k.isim[i] = ' ';
        liste[liste_adet++] = k;
    }
    fclose(file);
}

void skor_kaydet_baslat(const char *isim, int puan, int genislik, int yukseklik) {
    // Ayni isim (buyuk/kucuk harf farketmez) bir kez kullanilabilir: tarayicidaki kuralla ayni
    dosyadan_oku();
    for (int i = 0; i < liste_adet; i++) {
        if (ayni_isim(liste[i].isim, isim)) { kayit_durumu = DEPO_ISIM_ALINMIS; return; }
    }
    char guvenli[50];
    strncpy(guvenli, isim, sizeof guvenli - 1);
    guvenli[sizeof guvenli - 1] = '\0';
    for (int i = 0; guvenli[i] != '\0'; i++) if (guvenli[i] == ' ') guvenli[i] = '_';
    FILE *file = fopen(SKOR_DOSYASI, "a");
    if (file == NULL) { kayit_durumu = DEPO_HATA; return; }
    fprintf(file, "%s %d %d %d\n", guvenli, puan, genislik, yukseklik);
    fclose(file);
    kayit_durumu = DEPO_TAMAM;
}

DepoDurumu skor_kaydet_durumu(void) { return kayit_durumu; }

void skorlari_yukle_baslat(void) {
    dosyadan_oku();
    yukleme_durumu = DEPO_TAMAM;
}

DepoDurumu skorlari_yukle_durumu(void) { return yukleme_durumu; }

void skorlari_sifirla(void) {
    FILE *file = fopen(SKOR_DOSYASI, "w");
    if (file != NULL) fclose(file);
    liste_adet = 0;
}

#endif
