#include "Moves.hpp"

namespace chess_moves {

void Moves::doMove(chess_board::Move move) {}

void Moves::undoMove() {}

std::vector<chess_board::Move>
Moves::getPossibleMoves(chess_board::Color color) const {
  std::vector<chess_board::Move> allMoves;
  return allMoves;
}

chess_board::Bitboard Moves::getThreatSquares(chess_board::Color color) const {
  chess_board::Bitboard allThreats = 0;
  return allThreats;
}

} // namespace chess_moves