module;

#include <bit>
#include <cstdint>
#include <optional>
#include <vector>

export module chess.generator_helpers;

import chess.board_state;
import chess.core;
import chess.move;

export namespace chess::move_generator {

export struct PawnGenerator {
  struct Params {
    std::vector<chess::board::Move> &moveList;
    const chess::board::BoardState &boardState;
    chess::board::Bitboard pawns = 0ULL, promotionRank = 0ULL,
                           pawnStartRank = 0ULL, enPassantTargetSquare = 0ULL;
  };

  std::vector<chess::board::Move> &moveList;
  chess::board::Color color;
  chess::board::Bitboard pawns, empty, occupied, friendlyPieces, promotionRank,
      pawnStartRank, enPassantTargetSquare;

  PawnGenerator(const Params &params);
  void addPushOnceTargets() const;
  void addPushTwiceTargets() const;
  void addWestCaptureTargets() const;
  void addEastCaptureTargets() const;
  void addNormalMove(
      chess::board::Bitboard pieces, int8_t offset,
      chess::board::Move::Type type = chess::board::Move::Type::NORMAL) const;
  void addPromotionMove(chess::board::Bitboard pieces, int8_t offset) const;
  bool isWhite() const;
};

export struct KnightGenerator {
  struct Params {
    std::vector<chess::board::Move> &moveList;
    chess::board::BoardState boardState;
    chess::board::Bitboard knights = 0ULL;
  };

  std::vector<chess::board::Move> &moveList;
  chess::board::Color color;
  chess::board::Bitboard knights, empty, occupied, friendlyPieces;

  KnightGenerator(const Params &params);
  void addKnightTargets() const;
};

export struct SliderGenerator {
  struct Params {
    std::vector<chess::board::Move> &moveList;
    chess::board::BoardState boardState;
    chess::board::Bitboard bishops = 0ULL, rooks = 0ULL, queens = 0ULL;
  };

  std::vector<chess::board::Move> &moveList;
  chess::board::Color color;
  chess::board::Bitboard bishops, rooks, queens, empty, occupied,
      friendlyPieces;

  SliderGenerator(const Params &params);
  void addMoves(uint8_t startIndex, chess::board::Bitboard possibilities) const;
  void addDiagonalTargets(bool isQueen) const;
  void addOrthogonalTargets(bool isQueen) const;
};

export struct KingGenerator {
  struct Params {
    std::vector<chess::board::Move> &moveList;
    chess::board::BoardState boardState;
    chess::board::Bitboard kings = 0ULL, threats = 0ULL;
    chess::board::BoardState::Castles castles;
  };

  std::vector<chess::board::Move> &moveList;
  chess::board::Color color;
  chess::board::Bitboard kings, empty, occupied, friendlyPieces, threats;
  chess::board::BoardState::Castles castles;

  KingGenerator(const Params &params);
  void addMoves(
      uint8_t startIndex, chess::board::Bitboard possibilities,
      chess::board::Move::Type type = chess::board::Move::Type::NORMAL) const;
  void addKingTargets() const;
  void addCastleTargets() const;
};

} // namespace chess::move_generator