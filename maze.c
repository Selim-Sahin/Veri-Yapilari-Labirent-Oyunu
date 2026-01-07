#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "maze.h"
#include "stack.h"
#include "queue.h"

// --- 1. BELLEK YONETIMI ---
void labirenti_hazirla(Labirent *labirent, int w, int h) {
    labirent->genislik = w;
    labirent->yukseklik = h;
    labirent->harita = (Hucre**)malloc(h * sizeof(Hucre*));
    for (int i = 0; i < h; i++) {
        labirent->harita[i] = (Hucre*)malloc(w * sizeof(Hucre));
    }
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            labirent->harita[y][x].x = x;
            labirent->harita[y][x].y = y;
            labirent->harita[y][x].duvar_ust = true;
            labirent->harita[y][x].duvar_alt = true;
            labirent->harita[y][x].duvar_sol = true;
            labirent->harita[y][x].duvar_sag = true;
            labirent->harita[y][x].ziyaret_edildi = false;
            labirent->harita[y][x].yol_yonu = YON_YOK;
            labirent->harita[y][x].ozel_konum = false;
        }
    }
}

void labirenti_temizle(Labirent *labirent) {
    for (int i = 0; i < labirent->yukseklik; i++) free(labirent->harita[i]);
    free(labirent->harita);
}

// --- 2. CIZIM ---
void labirenti_ciz(Labirent *labirent) {
    ClearBackground(RAYWHITE);
    int hucre_boyutu = 30;
    int ust_bosluk = 60;
    int ekran_w = GetScreenWidth();
    int ekran_h = GetScreenHeight();
    int labirent_piksel_w = labirent->genislik * hucre_boyutu;
    int labirent_piksel_h = labirent->yukseklik * hucre_boyutu;
    int offset_x = (ekran_w - labirent_piksel_w) / 2;
    int kalan_yukseklik = ekran_h - ust_bosluk;
    int offset_y = ust_bosluk + (kalan_yukseklik - labirent_piksel_h) / 2;
    if (offset_x < 0) offset_x = 0;
    if (offset_y < ust_bosluk) offset_y = ust_bosluk;

    DrawRectangle(0, 0, ekran_w, ust_bosluk, GetColor(0x2C3E50FF));
    DrawLine(0, ust_bosluk, ekran_w, ust_bosluk, DARKGRAY);

    for (int y = 0; y < labirent->yukseklik; y++) {
        for (int x = 0; x < labirent->genislik; x++) {
            int ex = offset_x + (x * hucre_boyutu);
            int ey = offset_y + (y * hucre_boyutu);
            int cx = ex + hucre_boyutu / 2;
            int cy = ey + hucre_boyutu / 2;

            DrawRectangle(ex, ey, hucre_boyutu, hucre_boyutu, LIGHTGRAY);
            if (labirent->harita[y][x].duvar_ust) DrawRectangle(ex, ey, hucre_boyutu, 2, DARKGRAY);
            if (labirent->harita[y][x].duvar_alt) DrawRectangle(ex, ey + hucre_boyutu - 2, hucre_boyutu, 2, DARKGRAY);
            if (labirent->harita[y][x].duvar_sol) DrawRectangle(ex, ey, 2, hucre_boyutu, DARKGRAY);
            if (labirent->harita[y][x].duvar_sag) DrawRectangle(ex + hucre_boyutu - 2, ey, 2, hucre_boyutu, DARKGRAY);

            // --- BURAYI DEGISTIRIYORUZ ---
            // --- HINT (IPUCU) CIZIMI DUZELTILDI ---
            if (labirent->harita[y][x].yol_yonu != YON_YOK) {
                // Ucgenin boyutu (Yarıçap gibi düşünebilirsin, 8 piksel ideal)
                float s = 8.0f;
                Color okRengi = BLUE;

                // Koordinatlar (cx ve cy zaten karenin tam merkezi)
                Vector2 merkez = { (float)cx, (float)cy };

                // NOT: Raylib'de ucgenlerin gorunmesi icin noktalarin
                // SAAT YONUNUN TERSI (CCW) sirasiyla verilmesi gerekir.

                if (labirent->harita[y][x].yol_yonu == YON_YUKARI) {
                    // Üst, Sol-Alt, Sağ-Alt
                    DrawTriangle(
                        (Vector2){merkez.x, merkez.y - s},
                        (Vector2){merkez.x - s, merkez.y + s},
                        (Vector2){merkez.x + s, merkez.y + s},
                        okRengi
                    );
                }
                else if (labirent->harita[y][x].yol_yonu == YON_ASAGI) {
                    // Alt, Sağ-Üst, Sol-Üst (Sıralama Düzeltildi!)
                    DrawTriangle(
                        (Vector2){merkez.x, merkez.y + s},
                        (Vector2){merkez.x + s, merkez.y - s},
                        (Vector2){merkez.x - s, merkez.y - s},
                        okRengi
                    );
                }
                else if (labirent->harita[y][x].yol_yonu == YON_SOL) {
                    // Sol, Alt-Sağ, Üst-Sağ (Sıralama Düzeltildi!)
                    DrawTriangle(
                        (Vector2){merkez.x - s, merkez.y},
                        (Vector2){merkez.x + s, merkez.y + s},
                        (Vector2){merkez.x + s, merkez.y - s},
                        okRengi
                    );
                }
                else if (labirent->harita[y][x].yol_yonu == YON_SAG) {
                    // Sağ, Üst-Sol, Alt-Sol
                    DrawTriangle(
                        (Vector2){merkez.x + s, merkez.y},
                        (Vector2){merkez.x - s, merkez.y - s},
                        (Vector2){merkez.x - s, merkez.y + s},
                        okRengi
                    );
                }
            }
            // -----------------------------
            if (x == labirent->cikis_x && y == labirent->cikis_y) DrawRectangle(ex + 5, ey + 5, hucre_boyutu - 10, hucre_boyutu - 10, RED);
            if (x == labirent->oyuncu_x && y == labirent->oyuncu_y) DrawRectangle(ex + 5, ey + 5, hucre_boyutu - 10, hucre_boyutu - 10, GREEN);
        }
    }
}

// --- 3. LABIRENT URETIMI ---
void labirenti_uret(Labirent *labirent) {
    Stack yigin;
    stack_olustur(&yigin);
    int anlik_x = 0, anlik_y = 0;
    labirent->harita[anlik_y][anlik_x].ziyaret_edildi = true;

    while (true) {
        int komsular[4], komsu_sayisi = 0;
        if (anlik_y > 0 && !labirent->harita[anlik_y - 1][anlik_x].ziyaret_edildi) komsular[komsu_sayisi++] = 0;
        if (anlik_y < labirent->yukseklik - 1 && !labirent->harita[anlik_y + 1][anlik_x].ziyaret_edildi) komsular[komsu_sayisi++] = 1;
        if (anlik_x > 0 && !labirent->harita[anlik_y][anlik_x - 1].ziyaret_edildi) komsular[komsu_sayisi++] = 2;
        if (anlik_x < labirent->genislik - 1 && !labirent->harita[anlik_y][anlik_x + 1].ziyaret_edildi) komsular[komsu_sayisi++] = 3;

        if (komsu_sayisi > 0) {
            int r = rand() % komsu_sayisi;
            int yon = komsular[r];
            stack_ekle(&yigin, anlik_x, anlik_y);
            if (yon == 0) { labirent->harita[anlik_y][anlik_x].duvar_ust = false; labirent->harita[anlik_y - 1][anlik_x].duvar_alt = false; anlik_y--; }
            else if (yon == 1) { labirent->harita[anlik_y][anlik_x].duvar_alt = false; labirent->harita[anlik_y + 1][anlik_x].duvar_ust = false; anlik_y++; }
            else if (yon == 2) { labirent->harita[anlik_y][anlik_x].duvar_sol = false; labirent->harita[anlik_y][anlik_x - 1].duvar_sag = false; anlik_x--; }
            else if (yon == 3) { labirent->harita[anlik_y][anlik_x].duvar_sag = false; labirent->harita[anlik_y][anlik_x + 1].duvar_sol = false; anlik_x++; }
            labirent->harita[anlik_y][anlik_x].ziyaret_edildi = true;
        } else {
            if (stack_bos_mu(&yigin)) break;
            Nokta geri = stack_cikar(&yigin);
            anlik_x = geri.x;
            anlik_y = geri.y;
        }
    }
    labirent->cikis_x = labirent->genislik - 1;
    labirent->cikis_y = labirent->yukseklik - 1;
}

// --- 4. COZUM BULMA ---
void en_kisa_yolu_bul(Labirent *labirent) {
    for(int y=0; y<labirent->yukseklik; y++) {
        for(int x=0; x<labirent->genislik; x++) labirent->harita[y][x].yol_yonu = YON_YOK;
    }
    Queue kuyruk;
    queue_olustur(&kuyruk);
    bool *ziyaret = (bool*)calloc(labirent->genislik * labirent->yukseklik, sizeof(bool));
    Konum *ebeveyn = (Konum*)calloc(labirent->genislik * labirent->yukseklik, sizeof(Konum));

    queue_ekle(&kuyruk, labirent->oyuncu_x, labirent->oyuncu_y);
    ziyaret[labirent->oyuncu_y * labirent->genislik + labirent->oyuncu_x] = true;
    bool hedef_bulundu = false;

    while(!queue_bos_mu(&kuyruk)) {
        Konum anlik = queue_cikar(&kuyruk);
        if (anlik.x == labirent->cikis_x && anlik.y == labirent->cikis_y) {
            hedef_bulundu = true;
            break;
        }
        int dx[] = {0, 0, -1, 1};
        int dy[] = {-1, 1, 0, 0};
        for(int i=0; i<4; i++) {
            int yeni_x = anlik.x + dx[i];
            int yeni_y = anlik.y + dy[i];
            if (yeni_x >= 0 && yeni_x < labirent->genislik && yeni_y >= 0 && yeni_y < labirent->yukseklik) {
                bool duvar_var = false;
                if (i==0) duvar_var = labirent->harita[anlik.y][anlik.x].duvar_ust;
                if (i==1) duvar_var = labirent->harita[anlik.y][anlik.x].duvar_alt;
                if (i==2) duvar_var = labirent->harita[anlik.y][anlik.x].duvar_sol;
                if (i==3) duvar_var = labirent->harita[anlik.y][anlik.x].duvar_sag;
                int index = yeni_y * labirent->genislik + yeni_x;
                if (!duvar_var && !ziyaret[index]) {
                    ziyaret[index] = true;
                    queue_ekle(&kuyruk, yeni_x, yeni_y);
                    ebeveyn[index].x = anlik.x;
                    ebeveyn[index].y = anlik.y;
                }
            }
        }
    }
    labirent->en_kisa_yol_uzunlugu = 0;
    if (hedef_bulundu) {
        int curr_x = labirent->cikis_x, curr_y = labirent->cikis_y;
        while(curr_x != labirent->oyuncu_x || curr_y != labirent->oyuncu_y) {
            labirent->en_kisa_yol_uzunlugu++;
            int index = curr_y * labirent->genislik + curr_x;
            int parent_x = ebeveyn[index].x, parent_y = ebeveyn[index].y;
            int diff_x = curr_x - parent_x, diff_y = curr_y - parent_y;
            if (diff_y == -1) labirent->harita[parent_y][parent_x].yol_yonu = YON_YUKARI;
            else if (diff_y == 1) labirent->harita[parent_y][parent_x].yol_yonu = YON_ASAGI;
            else if (diff_x == -1) labirent->harita[parent_y][parent_x].yol_yonu = YON_SOL;
            else if (diff_x == 1) labirent->harita[parent_y][parent_x].yol_yonu = YON_SAG;
            curr_x = parent_x; curr_y = parent_y;
        }
    }
    free(ziyaret); free(ebeveyn);
}

// --- 5. SKOR FONKSIYONLARI ---

SkorDugumu* dugum_olustur(char *isim, int puan) {
    SkorDugumu *yeni = (SkorDugumu*)malloc(sizeof(SkorDugumu));
    strcpy(yeni->isim, isim);
    yeni->puan = puan;
    yeni->sol = NULL;
    yeni->sag = NULL;
    return yeni;
}

void bst_ekle(SkorDugumu **kok, SkorDugumu *yeni) {
    if (*kok == NULL) { *kok = yeni; return; }
    if (yeni->puan < (*kok)->puan) bst_ekle(&((*kok)->sol), yeni);
    else bst_ekle(&((*kok)->sag), yeni);
}

void bst_temizle(SkorDugumu *kok) {
    if (kok == NULL) return;
    bst_temizle(kok->sol);
    bst_temizle(kok->sag);
    free(kok);
}

void add_score(char *name, int point) {
    char safe_name[50];
    strcpy(safe_name, name);
    for(int i=0; safe_name[i] != '\0'; i++) if (safe_name[i] == ' ') safe_name[i] = '_';
    FILE *file = fopen("highscore.txt", "a");
    if (file != NULL) { fprintf(file, "%s %d\n", safe_name, point); fclose(file); }
}

void reset_scores() {
    FILE *file = fopen("highscore.txt", "w");
    if (file != NULL) fclose(file);
}

void agaci_diziye_dok(SkorDugumu *kok, SkorDugumu **dizi, int *index) {
    if (kok == NULL) return;
    agaci_diziye_dok(kok->sol, dizi, index);
    dizi[(*index)++] = kok;
    agaci_diziye_dok(kok->sag, dizi, index);
}

void merge(SkorDugumu **arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;
    SkorDugumu **L = (SkorDugumu**)malloc(n1 * sizeof(SkorDugumu*));
    SkorDugumu **R = (SkorDugumu**)malloc(n2 * sizeof(SkorDugumu*));
    for (int i=0; i<n1; i++) L[i] = arr[l + i];
    for (int j=0; j<n2; j++) R[j] = arr[m + 1 + j];
    int i=0, j=0, k=l;
    while (i < n1 && j < n2) {
        if (strcmp(L[i]->isim, R[j]->isim) <= 0) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    free(L); free(R);
}
void merge_sort(SkorDugumu **arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        merge_sort(arr, l, m);
        merge_sort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void swap(SkorDugumu **a, SkorDugumu **b) {
    SkorDugumu *t = *a; *a = *b; *b = t;
}
int partition(SkorDugumu **arr, int low, int high) {
    int pivot = arr[high]->puan;
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (arr[j]->puan > pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}
void quick_sort(SkorDugumu **arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quick_sort(arr, low, pi - 1);
        quick_sort(arr, pi + 1, high);
    }
}

int binary_search(SkorDugumu **arr, int l, int r, char *name) {
    if (r >= l) {
        int mid = l + (r - l) / 2;
        int res = strcmp(arr[mid]->isim, name);
        if (res == 0) return mid;
        if (res > 0) return binary_search(arr, l, mid - 1, name);
        return binary_search(arr, mid + 1, r, name);
    }
    return -1;
}

// GLOBAL DEGISKENLER
static int sort_mode = 0;
static char search_text[50] = "";
static int search_letter_count = 0;
// Artik index degil, "bulundu mu" bilgisini ve "kimi bulduk" bilgisini tutuyoruz.
static bool is_found = false;
static char found_name[50] = "";
static int search_result_state = -2; // -2: Bos, -1: Bulunamadi, 1: Bulundu

// --- YENI: ARAMA EKRANINI SIFIRLAYAN FONKSIYON ---
void reset_search_ui() {
    search_text[0] = '\0';
    search_letter_count = 0;
    search_result_state = -2;
    is_found = false;
    found_name[0] = '\0';
}

void show_scores_gui() {
    SkorDugumu *kok = NULL;
    char dosya_isim[50];
    int dosya_puan;
    int eleman_sayisi = 0;

    FILE *file = fopen("highscore.txt", "r");
    if (file != NULL) {
        while (fscanf(file, "%s %d", dosya_isim, &dosya_puan) == 2) {
            for(int i=0; dosya_isim[i] != '\0'; i++) if (dosya_isim[i] == '_') dosya_isim[i] = ' ';
            bst_ekle(&kok, dugum_olustur(dosya_isim, dosya_puan));
            eleman_sayisi++;
        }
        fclose(file);
    }

    DrawText("EN YUKSEK SKORLAR", 250, 40, 40, DARKBLUE);

    Rectangle btnPuan = {250, 90, 140, 30};
    Rectangle btnIsim = {410, 90, 140, 30};
    Color c1 = (sort_mode == 0) ? BLUE : LIGHTGRAY;
    Color c2 = (sort_mode == 1) ? BLUE : LIGHTGRAY;
    DrawRectangleRec(btnPuan, c1);
    DrawRectangleRec(btnIsim, c2);
    DrawText("SIRALA: PUAN", 260, 95, 18, WHITE);
    DrawText("SIRALA: ISIM", 420, 95, 18, WHITE);

    if (CheckCollisionPointRec(GetMousePosition(), btnPuan) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) sort_mode = 0;
    if (CheckCollisionPointRec(GetMousePosition(), btnIsim) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) sort_mode = 1;

    // --- ARAMA KUTUSU ---
    DrawText("ARA:", 250, 560, 20, BLACK);
    DrawRectangle(300, 555, 200, 30, LIGHTGRAY);
    DrawRectangleLines(300, 555, 200, 30, DARKGRAY);
    DrawText(search_text, 310, 560, 20, BLACK);

    Rectangle btnAra = {510, 555, 60, 30};
    DrawRectangleRec(btnAra, LIGHTGRAY);
    DrawRectangleLinesEx(btnAra, 1, DARKGRAY);
    DrawText("BUL", 520, 560, 20, BLACK);

    int key = GetCharPressed();
    while (key > 0) {
        if (((key >= 32) && (key <= 126)) && (search_letter_count < 20)) {
            search_text[search_letter_count] = (char)key;
            search_text[search_letter_count+1] = '\0';
            search_letter_count++;
        }
        key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (search_letter_count > 0) {
            search_letter_count--;
            search_text[search_letter_count] = '\0';
        }
    }

    SkorDugumu **dizi = NULL;
    if (eleman_sayisi > 0) {
        dizi = (SkorDugumu**)malloc(eleman_sayisi * sizeof(SkorDugumu*));
        int index = 0;
        agaci_diziye_dok(kok, dizi, &index);

        // ARAMA ISLEMI (Butona basilirsa)
        if (CheckCollisionPointRec(GetMousePosition(), btnAra) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // Binary search icin gecici dizi olusturup isme gore siraliyoruz
            SkorDugumu **aramaDizisi = (SkorDugumu**)malloc(eleman_sayisi * sizeof(SkorDugumu*));
            for(int i=0; i<eleman_sayisi; i++) aramaDizisi[i] = dizi[i];

            merge_sort(aramaDizisi, 0, eleman_sayisi - 1);
            int idx = binary_search(aramaDizisi, 0, eleman_sayisi - 1, search_text);

            if (idx != -1) {
                is_found = true;
                search_result_state = 1;
                strcpy(found_name, aramaDizisi[idx]->isim); // Bulunan ismi kaydet
            } else {
                is_found = false;
                search_result_state = -1;
            }
            free(aramaDizisi);
        }

        // GOSTERIM SIRALAMASI
        if (sort_mode == 0) quick_sort(dizi, 0, eleman_sayisi - 1); // Puan
        else merge_sort(dizi, 0, eleman_sayisi - 1); // Isim

        // DINAMIK SIRA HESAPLAMA
        // Eger birisi bulunduysa, su anki siralamada kacinci oldugunu bulalim
        int current_rank = -1;
        if (is_found) {
            for(int i=0; i<eleman_sayisi; i++) {
                if (strcmp(dizi[i]->isim, found_name) == 0) {
                    current_rank = i + 1;
                    break;
                }
            }
        }

        // LISTELEME
        int y_pos = 140;
        for (int i = 0; i < eleman_sayisi; i++) {
            if (y_pos > 500) break;
            DrawText(TextFormat("%d. %s", i + 1, dizi[i]->isim), 250, y_pos, 30, BLACK);
            DrawText(TextFormat("%d Puan", dizi[i]->puan), 550, y_pos, 30, DARKGREEN);
            y_pos += 40;
        }

        // SONUC MESAJI (DINAMIK)
        if (is_found && current_rank != -1) {
             DrawText(TextFormat("BULUNDU! SIRA: %d", current_rank), 600, 560, 20, GREEN);
        } else if (search_result_state == -1) {
             DrawText("YOK!", 600, 560, 20, RED);
        }

        free(dizi);
    } else {
        DrawText("Henuz kayitli skor yok.", 300, 150, 20, GRAY);
    }

    DrawLine(250, 130, 600, 130, DARKBLUE);
    bst_temizle(kok);
}