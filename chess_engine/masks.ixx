module;

import chess.core;
import <cstdint>;

export module chess.masks;

export namespace chess::board {

export constexpr Bitboard
    RANK_1 = 0xFF00000000000000ULL,
    RANK_2 = 0xFF000000000000ULL, RANK_3 = 0xFF0000000000ULL,
    RANK_4 = 0xFF00000000ULL, RANK_5 = 0xFF000000ULL, RANK_6 = 0xFF0000ULL,
    RANK_7 = 0xFF00ULL, RANK_8 = 0xFFULL,

    FILE_A = 0x0101010101010101ULL, FILE_B = 0x0202020202020202ULL,
    FILE_C = 0x0404040404040404ULL, FILE_D = 0x0808080808080808ULL,
    FILE_E = 0x1010101010101010ULL, FILE_F = 0x2020202020202020ULL,
    FILE_G = 0x4040404040404040ULL, FILE_H = 0x8080808080808080ULL,

    FILE_AB = 0x0303030303030303ULL, FILE_GH = 0xC0C0C0C0C0C0C0C0ULL,

    A1 = 0x0100000000000000ULL, D1 = 0x0800000000000000ULL,
    F1 = 0x2000000000000000ULL, H1 = 0x8000000000000000ULL,
    A8 = 0x0000000000000001ULL, D8 = 0x0000000000000008ULL,
    F8 = 0x0000000000000020ULL, H8 = 0x0000000000000080ULL,

    KNIGHT_SPAN = 0x0000000A1100110AULL, KING_SPAN = 0x00070507ULL,

    RANK_MASKS[8] = {0xFFULL,
                     0xFF00ULL,
                     0xFF0000ULL,
                     0xFF000000ULL,
                     0xFF00000000ULL,
                     0xFF0000000000,
                     0xFF000000000000ULL,
                     0xFF00000000000000ULL},

    FILE_MASKS[8] = {0x0101010101010101ULL, 0x0202020202020202ULL,
                     0x0404040404040404ULL, 0x0808080808080808ULL,
                     0x1010101010101010ULL, 0x2020202020202020ULL,
                     0x4040404040404040ULL, 0x8080808080808080ULL},

    DIAGONAL_MASKS1[15] = {0x1ULL,
                           0x102ULL,
                           0x10204ULL,
                           0x1020408ULL,
                           0x102040810ULL,
                           0x10204081020ULL,
                           0x1020408102040ULL,
                           0x102040810204080ULL,
                           0x204081020408000ULL,
                           0x408102040800000ULL,
                           0x810204080000000ULL,
                           0x1020408000000000ULL,
                           0x2040800000000000ULL,
                           0x4080000000000000ULL,
                           0x8000000000000000ULL},

    DIAGONAL_MASKS2[15] = {0x80ULL,
                           0x8040ULL,
                           0x804020ULL,
                           0x80402010ULL,
                           0x8040201008ULL,
                           0x804020100804ULL,
                           0x80402010080402ULL,
                           0x8040201008040201ULL,
                           0x4020100804020100ULL,
                           0x2010080402010000ULL,
                           0x1008040201000000ULL,
                           0x804020100000000ULL,
                           0x402010000000000ULL,
                           0x201000000000000ULL,
                           0x100000000000000ULL};

} // namespace chess::board