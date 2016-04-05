cmake_minimum_required(VERSION 3.3)
project(TP)

find_package (Threads)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

set(SOURCE_FILES Global.h main.cpp Partie.cpp Partie.h Joueur.cpp Joueur.h Reponse.cpp Reponse.h)
add_executable(TP ${SOURCE_FILES})
target_compile_features(TP PRIVATE cxx_range_for)
target_link_libraries(TP ${CMAKE_THREAD_LIBS_INIT})
