module;

import <cstdint>;

export module chess.shifts;

import chess.core;
import chess.masks;

export namespace chess::board {

export enum class Shift {
  NORTH = -8,
  SOUTH = 8,
  EAST = 1,
  WEST = -1,
  NORTH_EAST = NORTH + EAST,
  NORTH_WEST = NORTH + WEST,
  SOUTH_EAST = SOUTH + EAST,
  SOUTH_WEST = SOUTH + WEST,
  NORTH_DOUBLE = 2 * NORTH,
  SOUTH_DOUBLE = 2 * SOUTH,
  EAST_DOUBLE = 2 * EAST,
  WEST_DOUBLE = 2 * WEST
};

export [[nodiscard]] constexpr Bitboard shift(Bitboard bb, Shift shift) {
  if (shift == Shift::EAST || shift == Shift::NORTH_EAST ||
      shift == Shift::SOUTH_EAST) {
    bb &= ~chess::board::FILE_H;
  } else if (shift == Shift::WEST || shift == Shift::NORTH_WEST ||
             shift == Shift::SOUTH_WEST) {
    bb &= ~chess::board::FILE_A;
  }

  const int8_t shiftVal = static_cast<int8_t>(shift);
  return (shiftVal > 0) ? (bb << shiftVal) : (bb >> -shiftVal);
}

export [[nodiscard]] constexpr Bitboard shiftNorth(Bitboard bb) {
  return shift(bb, Shift::NORTH);
}
export [[nodiscard]] constexpr Bitboard shiftSouth(Bitboard bb) {
  return shift(bb, Shift::SOUTH);
}
export [[nodiscard]] constexpr Bitboard shiftWest(Bitboard bb) {
  return shift(bb, Shift::WEST);
}
export [[nodiscard]] constexpr Bitboard shiftEast(Bitboard bb) {
  return shift(bb, Shift::EAST);
}
export [[nodiscard]] constexpr Bitboard shiftNorthWest(Bitboard bb) {
  return shift(bb, Shift::NORTH_WEST);
}
export [[nodiscard]] constexpr Bitboard shiftNorthEast(Bitboard bb) {
  return shift(bb, Shift::NORTH_EAST);
}
export [[nodiscard]] constexpr Bitboard shiftSouthWest(Bitboard bb) {
  return shift(bb, Shift::SOUTH_WEST);
}
export [[nodiscard]] constexpr Bitboard shiftSouthEast(Bitboard bb) {
  return shift(bb, Shift::SOUTH_EAST);
}

} // namespace chess::board