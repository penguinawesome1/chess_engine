#pragma once
#include "board_state.hpp"
#include "generator_helpers.hpp"
#include "move.hpp"
#include <array>
#include <bit>
#include <map>
#include <optional>
#include <tuple>
#include <utility>
#include <vector>

namespace chess_move_generator {

std::vector<chess_board::Move>
getPossibleMoves(const chess_board::BoardState &boardState);
chess_board::Bitboard
getThreatSquares(const chess_board::Color color,
                 const chess_board::BoardState &boardState);

} // namespace chess_move_generator