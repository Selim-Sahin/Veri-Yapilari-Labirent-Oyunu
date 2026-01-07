#ifndef LABIRENT_OYUNU_STACK_H
#define LABIRENT_OYUNU_STACK_H

#include <stdbool.h>

// Koordinat yapisi: Stack'in icine sadece x ve y koyacagiz.
typedef struct {
    int x;
    int y;
} Nokta;

// Stack Yapisi
// Basit olmasi icin sabit boyutlu bir dizi kullaniyoruz.
// 1000 elemanlik hafiza labirentimiz icin fazlasiyla yeterli.
#define STACK_BOYUTU 10000

typedef struct {
    Nokta veriler[STACK_BOYUTU]; // Verilerin tutuldugu dizi
    int tepe;                    // En ustteki elemanin indisi (top)
} Stack;

// Fonksiyonlar
void stack_olustur(Stack *s);       // Stack'i baslatir
bool stack_bos_mu(Stack *s);        // Bos mu kontrol eder
bool stack_dolu_mu(Stack *s);       // Dolu mu kontrol eder
void stack_ekle(Stack *s, int x, int y); // Push
Nokta stack_cikar(Stack *s);        // Pop

#endif //LABIRENT_OYUNU_STACK_H