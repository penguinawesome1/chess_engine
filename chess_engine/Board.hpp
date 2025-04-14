#pragma once
#include "Core.hpp"
#include "Input.hpp"
#include "Moves.hpp"
#include <algorithm>
#include <bit>
#include <cstddef>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>

namespace chess_board {

class Board {
private:
  Pieces pieces_;
  chess_moves::Moves moves_;
  MoveCords bestMove_;
  std::size_t moveC_;
  bool isWhiteTurn_;
  char chessBoard_[8][8];
  const float pieceLocationValues[8][8];
  const float kingLocationValues[8][8];

  void printBoard() const;
  void arrayToBitboard();
  void initializeChess960();

  float minimax(const int depth, float alpha, float beta,
                const bool isRootCall);
  float evaluate(const int depth);
  bool isGameOver();
  bool areWhiteMoves();
  bool areBlackMoves();
  constexpr bool notEnoughPieces() const;
  constexpr int materialScore() const;
  constexpr float positionScore() const;

public:
  Board(const chess_moves::Moves &moves);
  void startGame();
};

} // namespace chess_board