#pragma once
#include "constants.hpp"
#include <bit>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace chess_board {

using Bitboard = std::uint64_t;

enum class Name { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, COUNT };
enum class Color { WHITE, BLACK, COUNT };

struct BoardAndIndex {
  chess_board::Bitboard board;
  std::uint8_t index;
};

[[nodiscard]] constexpr std::optional<BoardAndIndex>
getNextPiece(chess_board::Bitboard &board) {
  if (!board)
    return std::nullopt;

  std::uint8_t index = std::countr_zero(board);
  board &= (board - 1);
  return BoardAndIndex{1ULL << index, index};
}

[[nodiscard]] constexpr Color getOppositeColor(Color color) {
  return (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
}

// --- Bitboard Shift/Manipulation Helpers ---
// These functions shift a bitboard in a specific direction,
// handling board edges to prevent wrap-around.

[[nodiscard]] constexpr Bitboard shiftNorth(Bitboard bb) { return bb << 8; }
[[nodiscard]] constexpr Bitboard shiftSouth(Bitboard bb) { return bb >> 8; }
[[nodiscard]] constexpr Bitboard shiftEast(Bitboard bb) {
  return (bb & ~chess_board::constants::FILE_H) << 1;
}
[[nodiscard]] constexpr Bitboard shiftWest(Bitboard bb) {
  return (bb & ~chess_board::constants::FILE_A) >> 1;
}
[[nodiscard]] constexpr Bitboard shiftNorthEast(Bitboard bb) {
  return shiftNorth(shiftEast(bb));
}
[[nodiscard]] constexpr Bitboard shift_north_west(Bitboard bb) {
  return shiftNorth(shiftWest(bb));
}
[[nodiscard]] constexpr Bitboard shift_south_east(Bitboard bb) {
  return shiftSouth(shiftEast(bb));
}
[[nodiscard]] constexpr Bitboard shift_south_west(Bitboard bb) {
  return shiftSouth(shiftWest(bb));
}

// --- Coordinate and Square Representation ---

struct Cords {
  std::uint8_t x_, y_;

  constexpr Cords() : x_(0), y_(0) {}
  constexpr Cords(std::uint8_t x, std::uint8_t y) : x_(x), y_(7 - y) {}
  constexpr Cords(std::uint8_t index) : x_(index % 8), y_(7 - index / 8) {}

  [[nodiscard]] constexpr std::uint8_t getIndex() const { return y_ * 8 + x_; }
  [[nodiscard]] constexpr Bitboard getBitboard() const {
    return 1ULL << getIndex();
  }
  [[nodiscard]] constexpr std::string getAlgebraic() const {
    char fileChar = 'a' + y_;
    char rankChar = '1' + x_;
    return std::string(1, fileChar) + std::string(1, rankChar);
  }

  constexpr bool operator==(const Cords &other) const {
    return x_ == other.x_ && y_ == other.y_;
  }
  constexpr bool operator!=(const Cords &other) const {
    return !(*this == other);
  }
};

} // namespace chess_board