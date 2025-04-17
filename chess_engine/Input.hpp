#pragma once
#include "Core.hpp"
#include "Moves.hpp"
#include <algorithm>
#include <cctype>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>

namespace chess_input {

chess_board::GameParams gatherInputs();
chess_board::Move getPlayerMove(const chess_board::Color color,
                                const chess_moves::Moves &moves,
                                const chess_board::Pieces &pieces);

} // namespace chess_input