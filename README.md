# Veri Yapıları ile Labirent Oyunu

Bu proje, C dili ve Raylib kütüphanesi kullanılarak geliştirilmiş bir labirent oyunudur.

## Kullanılan Veri Yapıları ve Algoritmalar
- **Stack (Yığın):** Labirent üretimi (DFS) ve Hamle Geri Alma (Undo).
- **Queue (Kuyruk):** En kısa yol bulma (BFS).
- **Binary Search Tree (BST):** Skor tablosu yönetimi.
- **Sıralama:** Merge Sort ve Quick Sort.
- **Arama:** Binary Search.

## Nasıl Çalıştırılır?
1. Raylib kütüphanesinin kurulu olduğundan emin olun.
2. CMake kullanarak projeyi derleyin.
3. `main.exe` dosyasını çalıştırın.

## Tarayıcıda Oyna
Oyun, Emscripten ile WebAssembly'ye derlenip GitHub Pages'te yayınlanıyor:
**https://selim-sahin.github.io/Veri-Yapilari-Labirent-Oyunu/**

`main` dalına yapılan her push'ta `.github/workflows/pages.yml` oyunu yeniden derleyip yayınlar.
Elle derlemek için (emsdk kuruluyken):
```
emcmake cmake -B build-web -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release
cmake --build build-web
```
Not: Tarayıcı sürümünde skorlar sayfa yenilenince sıfırlanır.
