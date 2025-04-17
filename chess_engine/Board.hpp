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
#include <map>
#include <random>

namespace chess_board {

struct MinimaxParams {
  int depth;
  float alpha;
  float beta;
  bool isRootCall;
};

class Board {
private:
  Pieces pieces_;
  chess_moves::Moves moves_;
  Move bestMove_;
  std::size_t moveC_;
  bool isWhiteTurn_;
  static constexpr float pieceLocationValues[8][8]{
      {-.5f, -.4f, -.4f, -.4f, -.4f, -.4f, -.4f, -.5f},
      {-.4f, -.2f, .0f, .0f, .0f, .0f, -.2f, -.4f},
      {-.4f, .0f, .1f, .2f, .2f, .1f, .0f, -.4f},
      {-.4f, .0f, .2f, .25f, .25f, .2f, .0f, -.4f},
      {-.4f, .0f, .2f, .25f, .25f, .2f, .0f, -.4f},
      {-.4f, .0f, .1f, .2f, .2f, .1f, .0f, -.4f},
      {-.4f, -.2f, .0f, .0f, .0f, .0f, -.2f, -.4f},
      {-.5f, -.4f, -.4f, -.4f, -.4f, -.4f, -.4f, -.5f}};
  static constexpr float kingLocationValues[8][8]{
      {-.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f},
      {-.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f},
      {-.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f},
      {-.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f},
      {-.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f},
      {-.2f, -.2f, -.2f, -.2f, -.2f, -.2f, -.2f, -.2f},
      {.2f, .2f, .0f, .0f, .0f, .0f, .2f, .2f},
      {.2f, .3f, .1f, .0f, .0f, .1f, .3f, .2f}};
  char chessBoard_[8][8] = {{'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
                            {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
                            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                            {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
                            {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}};

  void printBoard() const;
  void arrayToBitboard();
  void initializeChess960();
  void handleTurn(const chess_board::GameParams &inputs);
  float minimax(const MinimaxParams &params);
  float evaluate(const int depth) const;
  bool isChecked(Color color) const;
  bool isCheckmate(Color color) const;
  bool isStalemate(Color color) const;
  bool isGameOver() const;
  bool hasLegalMoves(Color color) const;
  bool onlyKingsLeft() const;
  float materialScore() const;
  float positionScore() const;

public:
  Board(const chess_moves::Moves &moves);
  void startGame();
};

} // namespace chess_board