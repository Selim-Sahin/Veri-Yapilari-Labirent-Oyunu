#include "giris.h"
#include <math.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <string.h>
#include <ctype.h>

static bool basildi = false;
static bool birakildi = false;
static Vector2 basma_konumu = {0};
static Vector2 birakma_konumu = {0};

static bool dokunmatik = false;
static float olcek = 1.0f;  // canvas'in ekrandaki CSS genisligi / canvas pikseli
static int kare_no = 0;

void giris_guncelle(void) {
    kare_no++;

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

    dokunmatik = EM_ASM_INT({
        return (window.matchMedia && window.matchMedia('(pointer: coarse)').matches) ? 1 : 0;
    });

    // Bir yazi kutusu odaktayken olcegi dondur: klavye acilinca sayfa kuculur, canvas da
    // kuculur; olcek degisirse butonlar buyur ve duzen kullanicinin elinin altinda ziplar.
    int kutu_odakta = EM_ASM_INT({
        var a = document.activeElement;
        return (a && a.classList && a.classList.contains('kutu')) ? 1 : 0;
    });
    if (!kutu_odakta) {
        // Not: EM_ASM govdesinde ust seviyede virgul kullanmayin (makro argumani sanilir)
        double o = EM_ASM_DOUBLE({
            var c = Module.canvas;
            var r = c.getBoundingClientRect();
            return c.width ? r.width / c.width : 1;
        });
        if (o > 0.05) olcek = (float)o;
    }

    // Onceki karede kaydedilmeyen yazi kutularini gizle (ekran degisti demektir)
    EM_ASM({ if (Module.kutulariGizle) Module.kutulariGizle($0); }, kare_no);
}

bool tiklama_basildi(Rectangle alan) {
    return basildi && CheckCollisionPointRec(basma_konumu, alan);
}

bool tiklama_birakildi(Rectangle alan) {
    return birakildi && CheckCollisionPointRec(birakma_konumu, alan);
}

bool dokunmatik_mi(void) {
    return dokunmatik;
}

int dokunma_boyu(int normal) {
    if (!dokunmatik) return normal;
    // 48: yuvarlamadan sonra da 44 CSS pikselinin altina dusmesin.
    int gerekli = (int)ceilf(48.0f / olcek);
    // Cok dar ekranda (320 px) duzen 600 piksele sigsin diye ust sinir
    if (gerekli > 110) gerekli = 110;
    return (gerekli > normal) ? gerekli : normal;
}

// Input'tan gelen yaziyi kutunun kurallarina gore suzer (klavye yolundaki kurallarla ayni).
static void suz(YaziKutusu kutu, const char *gelen, char *metin, int en_fazla) {
    int n = 0;
    for (int i = 0; gelen[i] != '\0' && n < en_fazla; i++) {
        unsigned char c = (unsigned char)gelen[i];
        if (kutu == KUTU_GENISLIK || kutu == KUTU_YUKSEKLIK) {
            if (c < '0' || c > '9') continue;
        } else {
            if (c < 32 || c > 126) continue; // Turkce harf vb. ASCII disi: klavye yolunda da alinmiyor
            if (kutu == KUTU_ISIM && c == '_') c = ' ';
            // Isim: yalnizca harf, rakam, bosluk (Firestore kurali da bunu ister).
            // Turkce harfler shell.html'de daha once ASCII'ye cevrilir (s->s, g->g ...).
            if (kutu == KUTU_ISIM && !isalnum(c) && c != ' ') continue;
        }
        metin[n++] = (char)c;
    }
    metin[n] = '\0';
}

bool yazi_kutusu(YaziKutusu kutu, Rectangle alan, char *metin, int en_fazla) {
    EM_ASM({ Module.kutuYerlestir($0, $1, $2, $3, $4, $5); },
           kutu, alan.x, alan.y, alan.width, alan.height, kare_no);

    char gelen[128];
    int kirli = EM_ASM_INT({
        var yazi = Module.kutuOku($0);
        if (yazi === null) return 0;
        stringToUTF8(yazi, $1, $2);
        return 1;
    }, kutu, gelen, (int)sizeof gelen);
    if (kirli) {
        suz(kutu, gelen, metin, en_fazla);
    }
    // Suzulmus hali (ya da canvas'a klavyeyle yazilan) input'a geri yazilir
    EM_ASM({ Module.kutuYaz($0, UTF8ToString($1)); }, kutu, metin);

    return EM_ASM_INT({ return Module.kutuOdakta($0); }, kutu) != 0;
}

bool yazi_kutusu_enter(YaziKutusu kutu) {
    return EM_ASM_INT({ return Module.kutuEnter($0); }, kutu) != 0;
}

static OyunKomutu klavye_komutu(void);

OyunKomutu oyun_komutu(void) {
    OyunKomutu k = klavye_komutu();
    if (k != KOMUT_YOK) return k;
    // shell.html: ekran tuslari ve kaydirma Module.komutlar kuyruguna eklenir
    return (OyunKomutu)EM_ASM_INT({
        return (Module.komutlar && Module.komutlar.length) ? Module.komutlar.shift() : 0;
    });
}

void oyun_modu(bool acik) {
    EM_ASM({
        document.body.classList.toggle('oyunda', !!$0);
        if (Module.komutlar) Module.komutlar.length = 0; // onceki ekrandan kalan komut olmasin
    }, acik);
    menu_onay_goster(false);
}

void menu_onay_goster(bool onayda) {
    static int son = -1;
    if ((int)onayda == son) return;
    son = onayda;
    EM_ASM({
        var b = document.getElementById('menu-dugmesi');
        if (!b) return;
        b.textContent = $0 ? 'ÇIKILSIN MI?' : 'MENÜ';
        b.classList.toggle('onay', !!$0);
    }, onayda);
}

#else // Masaustu: eskisiyle ayni davranis

void giris_guncelle(void) {}

bool tiklama_basildi(Rectangle alan) {
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), alan);
}

bool tiklama_birakildi(Rectangle alan) {
    return IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), alan);
}

bool dokunmatik_mi(void) { return false; }

int dokunma_boyu(int normal) { return normal; }

bool yazi_kutusu(YaziKutusu kutu, Rectangle alan, char *metin, int en_fazla) {
    (void)kutu; (void)alan; (void)metin; (void)en_fazla;
    return false;
}

bool yazi_kutusu_enter(YaziKutusu kutu) { (void)kutu; return false; }

static OyunKomutu klavye_komutu(void);

OyunKomutu oyun_komutu(void) { return klavye_komutu(); }

void oyun_modu(bool acik) { (void)acik; }

void menu_onay_goster(bool onayda) { (void)onayda; }

#endif

// game.c'deki eski if-else zinciriyle ayni oncelik: W, S, A, D, H, Z
static OyunKomutu klavye_komutu(void) {
    if (IsKeyPressed(KEY_W)) return KOMUT_YUKARI;
    if (IsKeyPressed(KEY_S)) return KOMUT_ASAGI;
    if (IsKeyPressed(KEY_A)) return KOMUT_SOL;
    if (IsKeyPressed(KEY_D)) return KOMUT_SAG;
    if (IsKeyPressed(KEY_H)) return KOMUT_IPUCU;
    if (IsKeyPressed(KEY_Z)) return KOMUT_GERI_AL;
    return KOMUT_YOK;
}
