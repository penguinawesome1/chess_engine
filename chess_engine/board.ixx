module;

import chess.core;
import chess.board_state;
import chess.move;
import chess.input;
import <cstdint>;
import <vector>;

export module chess.board;

export namespace chess::board {

export class Board {
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
  void handleTurn(const chess::input::GameParams &inputs);

  MinimaxResult minimax(const MinimaxParams &params);
  bool isChecked(const Color color) const;
  bool isCheckmate(const Color color);
  bool isStalemate(const Color color);
  bool isGameOver(const Color color);
  bool hasLegalMoves(const Color color);
  float evaluate(const Color color, const uint8_t depth);

public:
  void startGame();
  static void printBoard(BoardState &boardState_);
};

} // namespace chess::board