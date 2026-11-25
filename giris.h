#ifndef LABIRENT_OYUNU_GIRIS_H
#define LABIRENT_OYUNU_GIRIS_H

#include <stdbool.h>
#include "raylib.h"

// Tiklamayi (fare, dokunma, kalem) tek yerden okur.
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

#endif //LABIRENT_OYUNU_GIRIS_H
