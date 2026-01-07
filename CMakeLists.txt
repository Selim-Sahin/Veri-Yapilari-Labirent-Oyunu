cmake_minimum_required(VERSION 3.21)
project(Veri_Yapilari_Projesi_Labirent_Oyunu C)

set(CMAKE_C_STANDARD 11)

# --- RAYLIB KUTUPHANESINI INDIR VE KUR ---
include(FetchContent)
FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/master.tar.gz
)
FetchContent_MakeAvailable(raylib)
# -----------------------------------------

# --- PROJE DOSYALARI ---
# DIKKAT: Buradaki "WIN32" kelimesi siyah konsol penceresini gizler!
add_executable(Veri_Yapilari_Projesi_Labirent_Oyunu WIN32
        main.c
        maze.c
        maze.h
        stack.h
        stack.c
        game.h
        game.c
        queue.h
        queue.c)

# Raylib kutuphanesini projeye bagla
# Windows'ta pencere yonetimi icin gerekli bazi sistem kutuphanelerini de ekliyoruz
if (WIN32)
    target_link_libraries(Veri_Yapilari_Projesi_Labirent_Oyunu raylib winmm gdi32)
else()
    target_link_libraries(Veri_Yapilari_Projesi_Labirent_Oyunu raylib)
endif()