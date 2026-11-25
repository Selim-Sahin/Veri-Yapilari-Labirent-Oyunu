#ifndef LABIRENT_OYUNU_SKOR_DEPO_H
#define LABIRENT_OYUNU_SKOR_DEPO_H

#include <stdbool.h>

// Skorlarin nerede saklandigini oyundan gizler.
//
// Masaustunde (exe): yerel highscore.txt dosyasi ("isim puan genislik yukseklik").
// Tarayicida: Firebase Firestore'daki ortak "skorlar" koleksiyonu (web/shell.html).
//   Sayfa yenilense de silinmez, herkes ayni tabloyu gorur. Firestore kurallari yalnizca
//   okumaya ve YENI kayit olusturmaya izin verir: silme/degistirme hicbir ziyaretciye acik
//   degil, tablo yalnizca Firebase konsolundan sifirlanir.
//
// Internet istekleri zaman alir; oyunun cizim dongusu beklerken durmasin diye islemler
// "baslat + her karede durumu sor" seklindedir (giris.c'deki tiklama sayaclari gibi).

#define SKOR_EN_FAZLA 500
#define ISIM_EN_AZ 2
#define ISIM_EN_FAZLA 15

typedef struct {
    char isim[50];
    int puan;
    int genislik;   // labirent boyutu; eski kayitlarda 0 (bilinmiyor)
    int yukseklik;
} SkorKaydi;

typedef enum {
    DEPO_BOSTA = -1,       // henuz islem baslatilmadi
    DEPO_BEKLIYOR = 0,     // istek yolda
    DEPO_TAMAM = 1,
    DEPO_ISIM_ALINMIS = 2, // bu isimle (buyuk/kucuk harf farketmez) zaten kayit var
    DEPO_HATA = 3          // baglanti / sunucu hatasi
} DepoDurumu;

// Ismi duzenler: bastaki/sondaki bosluklari atar, arka arkaya bosluklari teke indirir.
void isim_duzenle(char *isim);

// Kayit: isim zaten duzenlenmis olmali (isim_duzenle), ISIM_EN_AZ..ISIM_EN_FAZLA karakter.
void skor_kaydet_baslat(const char *isim, int puan, int genislik, int yukseklik);
DepoDurumu skor_kaydet_durumu(void);

// Tabloyu (puana gore en yuksek SKOR_EN_FAZLA kayit) yeniden yukler.
void skorlari_yukle_baslat(void);
DepoDurumu skorlari_yukle_durumu(void);

// Son basarili yuklemenin kayitlari (sirasiz; siralama maze.c'deki algoritmalarla yapilir).
const SkorKaydi *skor_listesi(int *adet);

// Tabloyu silme: yalnizca masaustunde (yerel dosya). Tarayicida bu islem yoktur.
void skorlari_sifirla(void);

#endif //LABIRENT_OYUNU_SKOR_DEPO_H
