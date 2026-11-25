#include "giris.h"
#include "raylib.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static bool dokunma_basladi = false;
static bool dokunma_bitti = false;

void giris_guncelle(void) {
#ifdef __EMSCRIPTEN__
    // web/shell.html canvas'taki touchstart/touchend olaylarini sayar.
    // Sayac kullaniyoruz cunku hizli bir dokunusun baslangici ve bitisi ayni kareye
    // dusebilir; o zaman "kac parmak var" sorusu dokunusu hic gormez.
    static int son_baslama = 0;
    static int son_bitis = 0;
    int baslama = EM_ASM_INT({ return Module.dokunmaBaslama | 0; });
    int bitis = EM_ASM_INT({ return Module.dokunmaBitis | 0; });
    dokunma_basladi = (baslama != son_baslama);
    dokunma_bitti = (bitis != son_bitis);
    son_baslama = baslama;
    son_bitis = bitis;
#endif
}

bool tiklama_basildi(void) {
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || dokunma_basladi;
}

bool tiklama_birakildi(void) {
    return IsMouseButtonReleased(MOUSE_LEFT_BUTTON) || dokunma_bitti;
}
