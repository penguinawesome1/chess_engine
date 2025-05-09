module;

#include <cstdint>
#include <string>

export module chess.cords;

import chess.core;

export namespace chess::board {

export struct Cords {
  uint8_t x_, y_;

  constexpr Cords() : x_(0), y_(0) {}
  constexpr Cords(uint8_t x, uint8_t y) : x_(x), y_(y) {}
  constexpr Cords(uint8_t index) : x_(index % 8), y_(index / 8) {}

  [[nodiscard]] constexpr uint8_t getIndex() const { return y_ * 8 + x_; }
  [[nodiscard]] constexpr Bitboard getBitboard() const {
    return 1ULL << getIndex();
  }
  [[nodiscard]] constexpr std::string getAlgebraic() const {
    char fileChar = x_ + 'a';
    char rankChar = 7 - y_ + '1';
    return std::string(1, fileChar) + std::string(1, rankChar);
  }

  constexpr bool operator==(const Cords &other) const {
    return x_ == other.x_ && y_ == other.y_;
  }
  constexpr bool operator!=(const Cords &other) const {
    return !(*this == other);
  }
};

} // namespace chess::board