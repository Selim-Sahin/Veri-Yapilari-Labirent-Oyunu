#ifndef LABIRENT_OYUNU_GIRIS_H
#define LABIRENT_OYUNU_GIRIS_H

#include <stdbool.h>

// Fare tiklamasi ile dokunmatik ekran dokunusunu tek yerden okur.
// Tarayicida raylib dokunmayi fare konumuna cevirir ama fare tusuna basilmis saymaz;
// bu fonksiyonlar dokunmayi da tiklama olarak sayar. Masaustunde sadece fareye bakar.

// Her karenin basinda bir kez cagrilmali (menu ve oyun donguleri).
void giris_guncelle(void);

// Sol tus bu karede basildi ya da ekrana dokunuldu.
bool tiklama_basildi(void);

// Sol tus bu karede birakildi ya da parmak ekrandan kalkti.
bool tiklama_birakildi(void);

#endif //LABIRENT_OYUNU_GIRIS_H
