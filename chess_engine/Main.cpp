/*
Purpose: Chess game with built in engine

Author: Owen Colley
Date: 4/17/25
*/

#include "Board.hpp"
#include "Moves.hpp"
#include <iostream>

int main() {
  chess_moves::Moves moves;
  chess_board::Board board(moves);
  board.startGame();

  return 0;
}