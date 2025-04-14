#pragma once
#include <cstdint>
#include <iostream>

namespace chess_board {

using Bitboard = uint64_t;

struct Pieces {
  Bitboard whitePawns;
  Bitboard whiteKnights;
  Bitboard whiteBishops;
  Bitboard whiteRooks;
  Bitboard whiteQueens;
  Bitboard whiteKings;

  Bitboard blackPawns;
  Bitboard blackKnights;
  Bitboard blackBishops;
  Bitboard blackRooks;
  Bitboard blackQueens;
  Bitboard blackKings;
};

enum class GameType { CHESS, CHESS960 };
enum class OpponentType { PLAYER, ENGINE };
enum class Color { WHITE, BLACK };

struct MoveCords {
  std::size_t x1;
  std::size_t y1;
  std::size_t x2;
  std::size_t y2;

  MoveCords() : x1(0), y1(0), x2(0), y2(0) {}

  void print() const {
    std::cout << "(" << x1 << ", " << y1 << ") -> (" << x2 << ", " << y2
              << ")\n";
  }
};

} // namespace chess_board