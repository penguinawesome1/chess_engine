#pragma once
#include "board_state.hpp"
#include "core.hpp"
#include "input.hpp"
#include "move.hpp"
#include "move_executor.hpp"
#include "move_generator.hpp"
#include <bit>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace chess_board {

class Board {
private:
  struct MinimaxParams {
    int depth;
    float alpha, beta;
    bool isRootCall;
  };

  struct MinimaxResult {
    float score;
    Move bestMove;
  };

  BoardState boardState_{};

  std::vector<Move> getAndPrintPossibleMoves() const;
  Move getEngineMove(const int depth);
  void handleTurn(const chess_input::GameParams &inputs);

  MinimaxResult minimax(const MinimaxParams &params);
  bool isChecked(const Color color) const;
  bool isCheckmate(const Color color);
  bool isStalemate(const Color color);
  bool isGameOver(const Color color);
  bool hasLegalMoves(const Color color);
  float evaluate(const Color color, const std::size_t depth);

public:
  void startGame();
  static void printBoard(BoardState &boardState_);
};

} // namespace chess_board