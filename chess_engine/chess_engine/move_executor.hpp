#pragma once
#include "board_state.hpp"
#include "constants.hpp"
#include "core.hpp"
#include "move.hpp"
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace chess_move_executor {

void doMove(chess_board::BoardState &boardState, const chess_board::Move &move);

} // namespace chess_move_executor