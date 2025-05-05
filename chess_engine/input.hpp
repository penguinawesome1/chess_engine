#pragma once
#include "move.hpp"
#include <iostream>
#include <vector>

namespace chess_input {

enum class OpponentType { HUMAN, ENGINE };

struct GameParams {
  OpponentType opponentType;
  chess_board::Color playerColor;
  int depth;
};

GameParams gatherInputs();
chess_board::Move
getPlayerMove(const std::vector<chess_board::Move> &possibleMoves);

} // namespace chess_input