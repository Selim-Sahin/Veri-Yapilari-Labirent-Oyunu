#include "giris.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

static bool basildi = false;
static bool birakildi = false;
static Vector2 basma_konumu = {0};
static Vector2 birakma_konumu = {0};

void giris_guncelle(void) {
    // Sayac kullaniyoruz: iki kare arasinda bas+birak birlikte gelse de ikisi de gorulur.
    static int son_basma = 0;
    static int son_birakma = 0;
    int basma = EM_ASM_INT({ return Module.tiklama ? Module.tiklama.basma : 0; });
    int birakma = EM_ASM_INT({ return Module.tiklama ? Module.tiklama.birakma : 0; });
    basildi = (basma != son_basma);
    birakildi = (birakma != son_birakma);
    son_basma = basma;
    son_birakma = birakma;
    if (basildi) {
        basma_konumu.x = (float)EM_ASM_DOUBLE({ return Module.tiklama.basmaX; });
        basma_konumu.y = (float)EM_ASM_DOUBLE({ return Module.tiklama.basmaY; });
    }
    if (birakildi) {
        birakma_konumu.x = (float)EM_ASM_DOUBLE({ return Module.tiklama.birakmaX; });
        birakma_konumu.y = (float)EM_ASM_DOUBLE({ return Module.tiklama.birakmaY; });
    }
}

bool tiklama_basildi(Rectangle alan) {
    return basildi && CheckCollisionPointRec(basma_konumu, alan);
}

bool tiklama_birakildi(Rectangle alan) {
    return birakildi && CheckCollisionPointRec(birakma_konumu, alan);
}

#else // Masaustu: eskisiyle ayni davranis

void giris_guncelle(void) {}

bool tiklama_basildi(Rectangle alan) {
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), alan);
}

bool tiklama_birakildi(Rectangle alan) {
    return IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), alan);
}

#endif
