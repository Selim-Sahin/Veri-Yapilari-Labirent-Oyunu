#ifndef LABIRENT_OYUNU_QUEUE_H
#define LABIRENT_OYUNU_QUEUE_H

#include <stdbool.h>

// Koordinat yapisi (Stack ile ayni mantik)
typedef struct {
    int x;
    int y;
    // BFS sirasinda "ben buraya hangi kareden geldim?" bilgisini tutmak icin
    // parent (ebeveyn) koordinatlarini da saklayabiliriz ama basitlik icin
    // bunu simdilik harici bir dizide tutacagiz.
} Konum;

#define QUEUE_BOYUTU 10000 // Labirent buyudugunde yetmesi icin genis tutalim

typedef struct {
    Konum veriler[QUEUE_BOYUTU];
    int bas; // Kuyrugun basi (cikis)
    int son; // Kuyrugun sonu (giris)
    int eleman_sayisi;
} Queue;

// Fonksiyonlar
void queue_olustur(Queue *q);
bool queue_bos_mu(Queue *q);
bool queue_dolu_mu(Queue *q);
void queue_ekle(Queue *q, int x, int y); // Enqueue
Konum queue_cikar(Queue *q);             // Dequeue

#endif //LABIRENT_OYUNU_QUEUE_H