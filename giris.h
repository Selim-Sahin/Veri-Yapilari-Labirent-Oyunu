#ifndef LABIRENT_OYUNU_GIRIS_H
#define LABIRENT_OYUNU_GIRIS_H

#include <stdbool.h>
#include "raylib.h"

// Tiklamayi (fare, dokunma, kalem) ve yazi kutularini tek yerden okur.
//
// Masaustunde (exe) raylib'in IsMouseButtonPressed/Released + GetMousePosition'i kullanilir.
//
// Tarayicida bunlara guvenmiyoruz: raylib tus durumunu olay geldikce ustune yazar ve
// "onceki durum"u ancak kare sonunda kaydeder. Bas ve birak ayni kareye duserse
// (RustDesk gibi uzak masaustu araclari, hizli dokunuslar) kenar tespiti tiklamayi kaybeder.
// Bunun yerine web/shell.html canvas'taki pointerdown/pointerup olaylarini sayar ve
// konumu olay aninda canvas pikseline cevirip saklar; burada her kare o sayaclar okunur.

// Her karenin basinda bir kez cagrilmali (menu ve oyun donguleri).
void giris_guncelle(void);

// Bu karede 'alan' icinde sol tusa basildi / ekrana dokunuldu.
bool tiklama_basildi(Rectangle alan);

// Bu karede 'alan' icinde sol tus birakildi / parmak kalkti.
bool tiklama_birakildi(Rectangle alan);

// --- Dokunmatik ekran ---

// Tarayicida ve ana isaretci parmak ise (telefon/tablet) true. Masaustunde (exe) hep false.
bool dokunmatik_mi(void);

// Dokunmatikte, ekranda en az ~44 CSS pikseli olacak canvas yuksekligi; 'normal'den kucukse
// 'normal'. Fare ile hep 'normal' doner, yani bilgisayardaki duzen degismez.
int dokunma_boyu(int normal);

// --- Yazi kutulari ---
// Tarayicida her kutunun ustunde seffaf, gercek bir HTML <input> durur (web/shell.html).
// Telefonda klavye ancak kullanici bir <input>'a dokununca acilir; canvas'a dokunmak yetmez.
// Kutuyu ve yaziyi C cizmeye devam eder, input yalnizca yaziyi tasir.
typedef enum {
    KUTU_GENISLIK = 0,
    KUTU_YUKSEKLIK,
    KUTU_ISIM,
    KUTU_ARAMA
} YaziKutusu;

// Ekranda gorunen her kutu icin her karede cagrilir: input'u 'alan'in ustune yerlestirir ve
// 'metin' ile esitler (input'ta yazildiysa metin oradan alinip suzulur, yoksa metin input'a
// yazilir). Kutu bu karede odaktaysa true. Masaustunde (exe) hicbir sey yapmaz, false doner.
bool yazi_kutusu(YaziKutusu kutu, Rectangle alan, char *metin, int en_fazla);

// Bu kutuda Enter'a basildi mi (bir kez true doner).
bool yazi_kutusu_enter(YaziKutusu kutu);

// --- Oyun komutlari ---
// Klavye (W/A/S/D/H/Z), tarayicidaki ekran tuslari ve parmakla kaydirma ayni komutlara
// donusur; oyun dongusu yalnizca bunlari isler. Sayilar shell.html'deki data-komut ile ayni.
typedef enum {
    KOMUT_YOK = 0,
    KOMUT_YUKARI = 1,
    KOMUT_ASAGI = 2,
    KOMUT_SOL = 3,
    KOMUT_SAG = 4,
    KOMUT_IPUCU = 5,
    KOMUT_GERI_AL = 6,
    KOMUT_MENU = 7
} OyunKomutu;

// Bu karenin komutu: once klavye (eski oncelik sirasiyla), yoksa tarayicidaki kuyruktan
// bir komut. Karede en fazla bir komut: hizli basislar kaybolmaz, sirayla islenir.
OyunKomutu oyun_komutu(void);

// Tarayicida oyun ekranina girip cikarken cagrilir: dokunmatik kontrol paneli yalnizca
// oyundayken gorunur, kaydirma yalnizca oyundayken hareket sayilir. Masaustunde bos.
void oyun_modu(bool acik);

// MENU iki basista cikar; ilk basistan sonra ekrandaki MENU butonu "CIKILSIN MI?" olur.
void menu_onay_goster(bool onayda);

#endif //LABIRENT_OYUNU_GIRIS_H
