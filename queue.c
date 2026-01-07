#include <stdio.h>
#include "queue.h"

void queue_olustur(Queue *q) {
    q->bas = 0;
    q->son = -1;
    q->eleman_sayisi = 0;
}

bool queue_bos_mu(Queue *q) {
    return q->eleman_sayisi == 0;
}

bool queue_dolu_mu(Queue *q) {
    return q->eleman_sayisi >= QUEUE_BOYUTU;
}

// Enqueue: Sona ekle
void queue_ekle(Queue *q, int x, int y) {
    if (queue_dolu_mu(q)) {
        // Normalde burada "dairesel kuyruk" yapilir ama proje icin duz mantik yeterli
        printf("HATA: Kuyruk dolu!\n");
        return;
    }
    q->son++;
    q->veriler[q->son].x = x;
    q->veriler[q->son].y = y;
    q->eleman_sayisi++;
}

// Dequeue: Bastan cikar
Konum queue_cikar(Queue *q) {
    Konum bos = {-1, -1};
    if (queue_bos_mu(q)) {
        return bos;
    }
    
    Konum veri = q->veriler[q->bas];
    q->bas++;
    q->eleman_sayisi--;
    
    // Eger kuyruk tamamen bosalirsa indisleri sifirlayalim (hafiza optimizasyonu)
    if (q->eleman_sayisi == 0) {
        q->bas = 0;
        q->son = -1;
    }
    
    return veri;
}