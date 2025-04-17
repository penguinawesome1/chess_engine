#pragma once
#include "Core.hpp"
#include <vector>

namespace chess_moves {

class Moves {
private:
  struct Masks {
    static constexpr chess_board::Bitboard RANK_1 = 0xFFL;
    static constexpr chess_board::Bitboard RANK_2 = 0xFF00L;
    static constexpr chess_board::Bitboard RANK_3 = 0xFF0000L;
    static constexpr chess_board::Bitboard RANK_4 = 0xFF000000L;
    static constexpr chess_board::Bitboard RANK_5 = 0xFF00000000L;
    static constexpr chess_board::Bitboard RANK_6 = 0xFF0000000000L;
    static constexpr chess_board::Bitboard RANK_7 = 0xFF000000000000L;
    static constexpr chess_board::Bitboard RANK_8 = 0xFF00000000000000L;

    static constexpr chess_board::Bitboard FILE_A = 0x0101010101010101L;
    static constexpr chess_board::Bitboard FILE_B = 0x0202020202020202L;
    static constexpr chess_board::Bitboard FILE_C = 0x0404040404040404L;
    static constexpr chess_board::Bitboard FILE_D = 0x0808080808080808L;
    static constexpr chess_board::Bitboard FILE_E = 0x1010101010101010L;
    static constexpr chess_board::Bitboard FILE_F = 0x2020202020202020L;
    static constexpr chess_board::Bitboard FILE_G = 0x4040404040404040L;
    static constexpr chess_board::Bitboard FILE_H = 0x8080808080808080L;

    static constexpr chess_board::Bitboard FILE_AB = FILE_A | FILE_B;
    static constexpr chess_board::Bitboard FILE_GH = FILE_G | FILE_H;

    static constexpr chess_board::Bitboard KNIGHT_SPAN = 0x4040200000204040ULL;
    static constexpr chess_board::Bitboard KING_SPAN = 0x70507ULL;

    static constexpr chess_board::Bitboard RANK_MASKS[8] = {
        0xFFL,         0xFF00L,         0xFF0000L,         0xFF000000L,
        0xFF00000000L, 0xFF0000000000L, 0xFF000000000000L, 0xFF00000000000000L};

    static constexpr chess_board::Bitboard FILE_MASKS[8] = {
        0x0101010101010101L, 0x0202020202020202L, 0x0404040404040404L,
        0x0808080808080808L, 0x1010101010101010L, 0x2020202020202020L,
        0x4040404040404040L, 0x8080808080808080L};

    static constexpr chess_board::Bitboard DIAGONAL_MASKS1[15] = {
        0x1L,
        0x102L,
        0x10204L,
        0x1020408L,
        0x102040810L,
        0x10204081020L,
        0x1020408102040L,
        0x102040810204080L,
        0x204081020408000L,
        0x408102040800000L,
        0x810204080000000L,
        0x1020408000000000L,
        0x2040800000000000L,
        0x4080000000000000L,
        0x8000000000000000L};

    static constexpr chess_board::Bitboard DIAGONAL_MASKS2[15] = {
        0x80L,
        0x8040L,
        0x804020L,
        0x80402010L,
        0x8040201008L,
        0x804020100804L,
        0x80402010080402L,
        0x8040201008040201L,
        0x4020100804020100L,
        0x2010080402010000L,
        0x1008040201000000L,
        0x804020100000000L,
        0x402010000000000L,
        0x201000000000000L,
        0x100000000000000L};
  };

  struct Castles {
    bool whiteShortCastle;
    bool whiteLongCastle;
    bool blackShortCastle;
    bool blackLongCastle;
  };

  struct Rooks {
    chess_board::Bitboard whiteLeftRook;
    chess_board::Bitboard whiteRightRook;
    chess_board::Bitboard blackLeftRook;
    chess_board::Bitboard blackRightRook;
  };

  struct moveData {
    char type1;
    char type2;
    char type3;
    chess_board::Bitboard board1;
    chess_board::Bitboard board2;
    chess_board::Bitboard board3;
    bool whiteShortCastle;
    bool whiteLongCastle;
    bool blackShortCastle;
    bool blackLongCastle;
  };

  std::vector<moveData> moveHistory;

public:
  Moves() = default;
  void doMove(chess_board::Move move);
  void undoMove();
  std::vector<chess_board::Move>
  getPossibleMoves(chess_board::Color color) const;
  chess_board::Bitboard getThreatSquares(chess_board::Color color) const;
}; // namespace chess_moves

} // namespace chess_moves