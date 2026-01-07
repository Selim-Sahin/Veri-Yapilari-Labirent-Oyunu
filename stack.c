#include <stdio.h>
#include "stack.h"

// Stack'i sifirlar
void stack_olustur(Stack *s) {
    s->tepe = -1; // -1 demek stack bos demek
}

bool stack_bos_mu(Stack *s) {
    return s->tepe == -1;
}

bool stack_dolu_mu(Stack *s) {
    return s->tepe >= STACK_BOYUTU - 1;
}

// Push islemi: Yeni koordinat ekler
void stack_ekle(Stack *s, int x, int y) {
    if (stack_dolu_mu(s)) {
        printf("HATA: Stack dolu! Daha fazla veri eklenemez.\n");
        return;
    }
    s->tepe++; // Bir basamak yukari cik
    s->veriler[s->tepe].x = x;
    s->veriler[s->tepe].y = y;
}

// Pop islemi: En ustteki koordinati alir ve siler
Nokta stack_cikar(Stack *s) {
    Nokta bos_nokta = {-1, -1}; // Hata durumunda donecek deger
    
    if (stack_bos_mu(s)) {
        printf("HATA: Stack bos! Cikarilacak veri yok.\n");
        return bos_nokta;
    }
    
    Nokta n = s->veriler[s->tepe]; // Veriyi al
    s->tepe--; // Bir basamak asagi in
    return n;
}