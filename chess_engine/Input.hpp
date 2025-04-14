#pragma once
#include "Core.hpp"
#include <cstddef>
#include <string>

namespace chess_input {

std::tuple<chess_board::GameType, chess_board::OpponentType, chess_board::Color,
           int>
gatherInputs();
chess_board::GameType getGameType();
chess_board::OpponentType getOpponent();
chess_board::Color getPlayerColor();
int getEngineDepth();

} // namespace chess_input