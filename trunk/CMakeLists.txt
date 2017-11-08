
cmake_minimum_required (VERSION 2.6)

#Name of our Project
project (PP1)

#L'executable 2048 crée par les fichiers game.c grid.c et grid.h
add_executable(2048_Ncurses src/game.c src/grid.c include/grid.h)
add_executable(2048_SDL src/game_sdl.c src/grid.c include/grid.h)
add_executable(EFFIC src/grid.c src/strat_eff.c )
add_executable(EFFICTEST src/grid.c src/strategy_efficient.c)
add_executable(FAST src/grid.c src/strategy.c)



#Configure le compilateur en -std=c99
add_definitions(-std=c99)

#Place l'executable 2048 dans le répertoire build
set(EXECUTABLE_OUTPUT_PATH build)

#Indication des répertoires des biblio
include_directories(include)

#liaison des biblio ncurses et math 
target_link_libraries(2048_Ncurses ncurses m)
target_link_libraries(2048_SDL m SDLmain SDL SDL_image SDL_ttf)
target_link_libraries(EFFIC ncurses m)
target_link_libraries(EFFICTEST ncurses m)
target_link_libraries(FAST ncurses m)


enable_testing()






