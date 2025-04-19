#pragma once
#include "Core.hpp"
#include <array>
#include <bit>
#include <map>
#include <optional>
#include <tuple>
#include <utility>
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

    Castles()
        : whiteShortCastle(true), whiteLongCastle(true), blackShortCastle(true),
          blackLongCastle(true) {}
  };

  struct Rooks {
    chess_board::Bitboard whiteLeftRook;
    chess_board::Bitboard whiteRightRook;
    chess_board::Bitboard blackLeftRook;
    chess_board::Bitboard blackRightRook;

    Rooks()
        : whiteLeftRook(0ULL), whiteRightRook(0ULL), blackLeftRook(0ULL),
          blackRightRook(0ULL) {}
  };

  struct MoveBoards {
    chess_board::Bitboard start;
    chess_board::Bitboard end;

    MoveBoards() : start(0ULL), end(0ULL) {}
    MoveBoards(chess_board::Bitboard start_, chess_board::Bitboard end_)
        : start(start_), end(end_) {}
  };

  enum class PieceName {
    WHITE_PAWNS,
    WHITE_KNIGHTS,
    WHITE_BISHOPS,
    WHITE_ROOKS,
    WHITE_QUEENS,
    WHITE_KINGS,
    BLACK_PAWNS,
    BLACK_KNIGHTS,
    BLACK_BISHOPS,
    BLACK_ROOKS,
    BLACK_QUEENS,
    BLACK_KINGS,
    NONE
  };

  struct MoveData {
    PieceName movedName;
    PieceName capturedName;
    PieceName specialName;
    chess_board::Bitboard movedBoard;
    chess_board::Bitboard capturedBoard;
    chess_board::Bitboard specialBoard;
    Castles castles;

    MoveData()
        : movedName(PieceName::NONE), capturedName(PieceName::NONE),
          specialName(PieceName::NONE), movedBoard(0ULL), capturedBoard(0ULL),
          specialBoard(0ULL), castles({}) {}
    MoveData(Castles castles_)
        : movedName(PieceName::NONE), capturedName(PieceName::NONE),
          specialName(PieceName::NONE), movedBoard(0ULL), capturedBoard(0ULL),
          specialBoard(0ULL), castles(castles_) {}
  };

  Masks masks_{};
  Castles castles_{};
  Rooks rooks_{};
  std::vector<MoveData> moveHistory_;

  static auto getMoveBoards(const chess_board::Move &localMove);
  static auto getBoardAndName(chess_board::Bitboard square,
                              chess_board::Pieces &localPieces)
      -> std::optional<
          std::pair<std::reference_wrapper<chess_board::Bitboard>, PieceName>>;
  static auto getBoardsAndNamePromotion(chess_board::Move::Type localType,
                                        chess_board::Pieces &pieces)
      -> std::optional<
          std::tuple<std::reference_wrapper<chess_board::Bitboard>,
                     std::reference_wrapper<chess_board::Bitboard>, PieceName>>;
  auto getBoardAndNameCastle(chess_board::Move::Type localType,
                             chess_board::Pieces &pieces)
      -> std::optional<std::tuple<std::reference_wrapper<chess_board::Bitboard>,
                                  chess_board::Bitboard, PieceName>>;
  void handleCapture(chess_board::Bitboard endSquare, MoveData moveData,
                     chess_board::Pieces &pieces);
  void handleMovedPiece(MoveBoards moveBoards, MoveData moveData,
                        chess_board::Pieces pieces);
  void handlePromotion(chess_board::Move::Type localType,
                       chess_board::Bitboard endSquare, MoveData moveData,
                       chess_board::Pieces pieces);
  void handleCastling(chess_board::Move::Type localType, MoveData moveData,
                      chess_board::Pieces pieces);
  void possibleP(std::vector<chess_board::Move> &allMoves,
                 const chess_board::Color color,
                 chess_board::Bitboard cantCapture, chess_board::Bitboard empty,
                 chess_board::Bitboard enPassant,
                 chess_board::Bitboard pawns) const;
  void possibleN(std::vector<chess_board::Move> &allMoves,
                 const chess_board::Color color,
                 const chess_board::Bitboard cantCapture,
                 chess_board::Bitboard knights) const;
  void possibleSliderMoves(std::vector<chess_board::Move> &allMoves,
                           const chess_board::Color color,
                           const PieceName pieceType,
                           const chess_board::Bitboard cantCapture,
                           const chess_board::Bitboard empty,
                           chess_board::Bitboard pieces) const;
  static constexpr chess_board::Bitboard
  hypQuint(const chess_board::Bitboard empty,
           const chess_board::Bitboard square,
           const chess_board::Bitboard mask);
  static constexpr chess_board::Bitboard reverse(chess_board::Bitboard b);
  void possibleK(std::vector<chess_board::Move> &allMoves,
                 const chess_board::Color color,
                 const chess_board::Bitboard cantCapture,
                 const chess_board::Bitboard empty,
                 const chess_board::Bitboard threats,
                 const chess_board::Bitboard king) const;

public:
  Moves() = default;
  void initRooks(const chess_board::Pieces &pieces);
  void doMove(const chess_board::Move &move, chess_board::Pieces &pieces);
  void undoMove(chess_board::Pieces pieces);
  std::vector<chess_board::Move>
  getPossibleMoves(chess_board::Color color, chess_board::Pieces pieces) const;
  chess_board::Bitboard
  getThreatSquares(chess_board::Color color,
                   const chess_board::Pieces &pieces) const;
}; // namespace chess_moves

} // namespace chess_moves