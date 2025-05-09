module;

import <bit>;
import <cstdint>;
import <optional>;

export module chess.core;

export namespace chess::board {

export using Bitboard = uint64_t;
export enum class Name { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, COUNT };
export enum class Color { WHITE, BLACK, COUNT };

export struct BoardAndIndex {
  chess::board::Bitboard board;
  uint8_t index;
};

export [[nodiscard]] constexpr auto getNextPiece(chess::board::Bitboard &board)
    -> std::optional<BoardAndIndex> {
  if (!board)
    return std::nullopt;

  uint8_t index = std::countr_zero(board);
  board &= (board - 1);
  return BoardAndIndex{1ULL << index, index};
}

export [[nodiscard]] constexpr Color getOppositeColor(Color color) {
  return (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
}

} // namespace chess::board