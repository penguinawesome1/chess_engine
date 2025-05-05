/**
 * @file main.cpp
 * @brief Entry point for the Chess game application with a built-in engine.
 *
 * @author Owen Colley
 * @date May 4, 2025
 */

#include "board.hpp"

int main() {
  chess_board::Board board;
  board.startGame();

  return 0;
}