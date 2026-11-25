cmake_minimum_required(VERSION 3.21)
project(Veri_Yapilari_Projesi_Labirent_Oyunu C)

set(CMAKE_C_STANDARD 11)

# --- RAYLIB KUTUPHANESINI INDIR VE KUR ---
include(FetchContent)
FetchContent_Declare(
        raylib
        # master'in sabit bir commit'i: raylib'in web dokunma/fare davranisi bizden habersiz degismesin
        URL https://github.com/raysan5/raylib/archive/7cbadefd060c5ba1a78197d129f83a60df42f741.tar.gz
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
        queue.c
        giris.h
        giris.c)

# Raylib kutuphanesini projeye bagla
# Windows'ta pencere yonetimi icin gerekli bazi sistem kutuphanelerini de ekliyoruz
if (WIN32)
    target_link_libraries(Veri_Yapilari_Projesi_Labirent_Oyunu raylib winmm gdi32)
else()
    target_link_libraries(Veri_Yapilari_Projesi_Labirent_Oyunu raylib)
endif()

# --- WEB (EMSCRIPTEN) DERLEMESI ---
# emcmake cmake -B build-web -DPLATFORM=Web ile derlendiginde index.html/js/wasm uretir.
# ASYNCIFY: oyundaki ic ice while donguleri tarayiciyi kilitlemeden calissin diye.
if (EMSCRIPTEN)
    set_target_properties(Veri_Yapilari_Projesi_Labirent_Oyunu PROPERTIES
            OUTPUT_NAME "index"
            SUFFIX ".html")
    target_link_options(Veri_Yapilari_Projesi_Labirent_Oyunu PRIVATE
            -sUSE_GLFW=3
            -sASYNCIFY
            -sALLOW_MEMORY_GROWTH=1
            # Stack/Queue yapilari ~80 KB'lik dizileri yerel degisken olarak tutuyor.
            # Emscripten'in varsayilan yigini 64 KB (Windows'ta 1 MB), bu yuzden buyutuyoruz.
            -sSTACK_SIZE=4MB
            -sASYNCIFY_STACK_SIZE=65536
            # giris.c'deki EM_ASM bloklari (yazi kutulari) bu iki yardimciyi kullaniyor
            "-sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE=$stringToUTF8,$UTF8ToString"
            --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html)
    # shell.html degisince sayfa yeniden uretilsin
    set_property(TARGET Veri_Yapilari_Projesi_Labirent_Oyunu APPEND PROPERTY
            LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html)
    # Sekme logosu ve iOS ana ekran ikonu index.html'in yanina kopyalanir
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/web/favicon.svg
            ${CMAKE_CURRENT_BINARY_DIR}/favicon.svg COPYONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/web/apple-touch-icon.png
            ${CMAKE_CURRENT_BINARY_DIR}/apple-touch-icon.png COPYONLY)
endif()