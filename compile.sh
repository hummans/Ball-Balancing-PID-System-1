
#optimization level: fast
g++ -std=c++11 -Wall -ggdb3 serial/serial.c ball_tracker/ball_physic.c ball_tracker/ball_tracker.cpp settings/file_handler.c modes/standard_mode.cpp dispatcher.cpp -Ofast -o run `pkg-config --cflags --libs opencv`

