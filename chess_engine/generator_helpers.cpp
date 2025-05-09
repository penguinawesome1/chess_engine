module;

#include <bit>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <vector>

module chess.generator_helpers;

import chess.board_state;
import chess.masks;
import chess.core;
import chess.cords;
import chess.move_generator;
import chess.shifts;

namespace chess::move_generator {

namespace {

struct HypQuintParams {
  chess::board::Bitboard empty, square, mask;
};

constexpr chess::board::Bitboard reverse(chess::board::Bitboard b) {
  b = (b & 0x5555555555555555) << 1 | ((b >> 1) & 0x5555555555555555);
  b = (b & 0x3333333333333333) << 2 | ((b >> 2) & 0x3333333333333333);
  b = (b & 0x0f0f0f0f0f0f0f0f) << 4 | ((b >> 4) & 0x0f0f0f0f0f0f0f0f);
  b = (b & 0x00ff00ff00ff00ff) << 8 | ((b >> 8) & 0x00ff00ff00ff00ff);

  return (b << 48) | ((b & 0xffff0000) << 16) | ((b >> 16) & 0xffff0000) |
         (b >> 48);
}

constexpr chess::board::Bitboard hypQuint(const HypQuintParams &params) {
  return ((((params.mask & ~params.empty) - params.square * 2) ^
           reverse(reverse(params.mask & ~params.empty) -
                   (reverse(params.square) * 2)))) &
         params.mask;
}

} // namespace

#pragma region pawns
PawnGenerator::PawnGenerator(const Params &params)
    : color(params.boardState.getTurnColor()), pawns(params.pawns),
      empty(params.boardState.getEmpty()), occupied(~empty),
      friendlyPieces(params.boardState.getPieces(color)),
      promotionRank(params.promotionRank), pawnStartRank(params.pawnStartRank),
      enPassantTargetSquare(params.enPassantTargetSquare),
      moveList(params.moveList) {}

void PawnGenerator::addPushOnceTargets() const {
  const chess::board::Shift myShift =
      isWhite() ? chess::board::Shift::NORTH : chess::board::Shift::SOUTH;
  const chess::board::Bitboard targets =
      chess::board::shift(pawns, myShift) & empty;
  const int8_t shiftNum = static_cast<int8_t>(myShift);

  addNormalMove(targets, shiftNum);
  addPromotionMove(targets, shiftNum);
}

void PawnGenerator::addPushTwiceTargets() const {
  const chess::board::Shift myShift =
      isWhite() ? chess::board::Shift::NORTH : chess::board::Shift::SOUTH;
  const chess::board::Bitboard targets =
      shift(shift(pawns & pawnStartRank, myShift) & empty, myShift) & empty;

  addNormalMove(targets, 2 * static_cast<int8_t>(myShift));
}

void PawnGenerator::addWestCaptureTargets() const {
  const chess::board::Shift shiftDirection =
      isWhite() ? chess::board::Shift::NORTH_WEST
                : chess::board::Shift::SOUTH_WEST;
  const chess::board::Bitboard targets =
      chess::board::shift(pawns, shiftDirection) & ~friendlyPieces;
  const int8_t shiftNum = static_cast<int8_t>(shiftDirection);

  addNormalMove(targets & occupied, shiftNum);
  addPromotionMove(targets & occupied, shiftNum);
  addNormalMove(targets & enPassantTargetSquare, shiftNum,
                chess::board::Move::Type::EN_PASSANT);
}

void PawnGenerator::addEastCaptureTargets() const {
  const chess::board::Shift shiftDirection =
      isWhite() ? chess::board::Shift::NORTH_EAST
                : chess::board::Shift::SOUTH_EAST;
  const chess::board::Bitboard targets =
      chess::board::shift(pawns, shiftDirection) & ~friendlyPieces;
  const int8_t shiftNum = static_cast<int8_t>(shiftDirection);

  addNormalMove(targets & occupied, shiftNum);
  addPromotionMove(targets & occupied, shiftNum);
  addNormalMove(targets & enPassantTargetSquare, shiftNum,
                chess::board::Move::Type::EN_PASSANT);
}

void PawnGenerator::addNormalMove(chess::board::Bitboard pieces, int8_t offset,
                                  chess::board::Move::Type type) const {
  chess::board::Bitboard currentPieces = pieces & ~promotionRank;

  while (auto pieceInfo = chess::board::getNextPiece(currentPieces)) {
    uint8_t index = pieceInfo->index;
    if (index - offset < 0)
      throw std::logic_error(
          "Error: 0ffset or index is incorrect causing underflow.");
    moveList.push_back(
        {chess::board::Cords(static_cast<uint8_t>(index - offset)),
         chess::board::Cords(index), type});
  }
}

void PawnGenerator::addPromotionMove(chess::board::Bitboard pieces,
                                     int8_t offset) const {
  const chess::board::Move::Type basePromotionType =
      isWhite() ? chess::board::Move::Type::WHITE_PAWN_PROMOTION_KNIGHT
                : chess::board::Move::Type::BLACK_PAWN_PROMOTION_KNIGHT;

  chess::board::Bitboard currentPieces = pieces & promotionRank;

  while (auto pieceInfo = chess::board::getNextPiece(currentPieces)) {
    for (uint8_t i = 0; i < 4; ++i) {
      uint8_t index = pieceInfo->index;
      if (index - offset < 0)
        throw std::logic_error(
            "Error: 0ffset or index is incorrect causing underflow.");
      moveList.push_back(
          {chess::board::Cords(static_cast<uint8_t>(index - offset)),
           chess::board::Cords(index),
           static_cast<chess::board::Move::Type>(
               static_cast<uint8_t>(basePromotionType) + i)});
    }
  }
}

bool PawnGenerator::isWhite() const {
  return color == chess::board::Color::WHITE;
}
#pragma endregion

#pragma region knights
KnightGenerator::KnightGenerator(const Params &params)
    : moveList(params.moveList), color(params.boardState.getTurnColor()),
      knights(params.knights), empty(params.boardState.getEmpty()),
      occupied(~empty), friendlyPieces(params.boardState.getPieces(color)) {}

void KnightGenerator::addKnightTargets() const {
  chess::board::Bitboard currentKnights = knights, possibilities;
  uint8_t knightIndex;
  uint8_t possibilitiesIndex;
  bool isOnLeftHalf;

  while (auto pieceInfo = chess::board::getNextPiece(currentKnights)) {
    knightIndex = pieceInfo->index;
    isOnLeftHalf = knightIndex % 8 < 4;
    possibilities =
        (knightIndex > 18 ? chess::board::KNIGHT_SPAN << (knightIndex - 18)
                          : chess::board::KNIGHT_SPAN >> (18 - knightIndex)) &
        (isOnLeftHalf ? ~chess::board::FILE_GH : ~chess::board::FILE_AB) &
        ~friendlyPieces;

    while (auto pieceInfo = chess::board::getNextPiece(possibilities)) {
      possibilitiesIndex = pieceInfo->index;
      moveList.push_back({knightIndex, possibilitiesIndex});
    }
  }
}
#pragma endregion

#pragma region sliders
SliderGenerator::SliderGenerator(const Params &params)
    : moveList(params.moveList), color(params.boardState.getTurnColor()),
      bishops(params.bishops), rooks(params.rooks), queens(params.queens),
      empty(params.boardState.getEmpty()), occupied(~empty),
      friendlyPieces(params.boardState.getPieces(color)) {}

void SliderGenerator::addMoves(uint8_t startIndex,
                               chess::board::Bitboard possibilities) const {
  uint8_t possibilitiesIndex = 0;
  possibilities &= ~friendlyPieces;

  while (auto pieceInfo = chess::board::getNextPiece(possibilities)) {
    moveList.push_back({startIndex, pieceInfo->index});
  }
}

void SliderGenerator::addDiagonalTargets(bool isQueen) const {
  chess::board::Bitboard pieces = isQueen ? queens : bishops,
                         smallestPiece = 0ULL, possibilities = 0ULL;
  uint8_t pieceIndex = 0;

  while (auto pieceInfo = chess::board::getNextPiece(pieces)) {
    pieceIndex = pieceInfo->index;
    smallestPiece = pieceInfo->board;

    possibilities = hypQuint(
        {.empty = empty,
         .square = smallestPiece,
         .mask =
             chess::board::DIAGONAL_MASKS1[pieceIndex % 8 + pieceIndex / 8]});
    possibilities |=
        hypQuint({.empty = empty,
                  .square = smallestPiece,
                  .mask = chess::board::DIAGONAL_MASKS2[7 - pieceIndex % 8 +
                                                        pieceIndex / 8]});
  }

  addMoves(pieceIndex, possibilities);
}

void SliderGenerator::addOrthogonalTargets(bool isQueen) const {
  chess::board::Bitboard pieces = isQueen ? queens : rooks,
                         smallestPiece = 0ULL, possibilities = 0ULL;
  uint8_t pieceIndex = 0;

  while (auto pieceInfo = chess::board::getNextPiece(pieces)) {
    pieceIndex = pieceInfo->index;
    smallestPiece = pieceInfo->board;

    possibilities =
        hypQuint({.empty = empty,
                  .square = smallestPiece,
                  .mask = chess::board::FILE_MASKS[pieceIndex % 8]});
    possibilities |=
        hypQuint({.empty = empty,
                  .square = smallestPiece,
                  .mask = chess::board::RANK_MASKS[pieceIndex / 8]});

    addMoves(pieceIndex, possibilities);
  }
}

#pragma endregion

#pragma region kings
KingGenerator::KingGenerator(const Params &params)
    : moveList(params.moveList), color(params.boardState.getTurnColor()),
      kings(params.kings), empty(params.boardState.getEmpty()),
      occupied(~empty), friendlyPieces(params.boardState.getPieces(color)),
      threats(params.threats), castles(params.castles) {}

void KingGenerator::addMoves(uint8_t startIndex,
                             chess::board::Bitboard possibilities,
                             chess::board::Move::Type type) const {
  while (auto pieceInfo = chess::board::getNextPiece(possibilities)) {
    moveList.push_back({startIndex, pieceInfo->index, type});
  }
}

void KingGenerator::addKingTargets() const {
  chess::board::Bitboard pieces = kings, possibilities;
  uint8_t kingIndex;

  while (auto pieceInfo = chess::board::getNextPiece(pieces)) {
    kingIndex = pieceInfo->index;
    possibilities =
        (kingIndex > 9 ? chess::board::KING_SPAN << (kingIndex - 9)
                       : chess::board::KING_SPAN >> (9 - kingIndex)) &
        (kingIndex % 8 < 4 ? ~chess::board::FILE_GH : ~chess::board::FILE_AB) &
        ~friendlyPieces & ~threats;

    while (auto pieceInfo = chess::board::getNextPiece(possibilities)) {
      moveList.push_back({kingIndex, pieceInfo->index});
    }
  }
}

void KingGenerator::addCastleTargets() const {
  chess::board::Bitboard pieces = kings, smallestPiece = 0ULL;
  uint8_t pieceIndex = 0;

  while (auto pieceInfo = chess::board::getNextPiece(pieces)) {
    pieceIndex = pieceInfo->index;
    smallestPiece = pieceInfo->board;

    const bool isKingFrontRank = smallestPiece & chess::board::RANK_1,
               isKingBackRank = smallestPiece & chess::board::RANK_8;

    if (castles.whiteLong && isKingFrontRank)
      addMoves(pieceIndex,
               empty & ~threats & chess::board::shiftWest(empty & ~threats) &
                   chess::board::shiftEast(empty & ~threats) &
                   shift(smallestPiece, chess::board::Shift::WEST_DOUBLE),
               chess::board::Move::Type::WHITE_CASTLE_QUEENSIDE);

    if (castles.whiteShort && isKingFrontRank)
      addMoves(pieceIndex,
               empty & ~threats & chess::board::shiftEast(empty & ~threats) &
                   shift(smallestPiece, chess::board::Shift::EAST_DOUBLE),
               chess::board::Move::Type::WHITE_CASTLE_KINGSIDE);

    if (castles.blackShort && isKingBackRank)
      addMoves(pieceIndex,
               empty & ~threats & chess::board::shiftWest(empty & ~threats) &
                   shift(smallestPiece, chess::board::Shift::WEST_DOUBLE),
               chess::board::Move::Type::BLACK_CASTLE_KINGSIDE);

    if (castles.blackLong && isKingBackRank)
      addMoves(pieceIndex,
               empty & ~threats & chess::board::shiftWest(empty & ~threats) &
                   chess::board::shiftEast(empty & ~threats) &
                   shift(smallestPiece, chess::board::Shift::EAST_DOUBLE),
               chess::board::Move::Type::BLACK_CASTLE_QUEENSIDE);
  }
}
#pragma endregion

auto getThreatSquares(const chess::board::Color color,
                      const chess::board::BoardState &boardState)
    -> chess::board::Bitboard {
  const bool isWhite = (color == chess::board::Color::WHITE);

  chess::board::Bitboard
      pawns = boardState.getPieces(color, chess::board::Name::PAWN),
      knights = boardState.getPieces(color, chess::board::Name::KNIGHT),
      bishops = boardState.getPieces(color, chess::board::Name::BISHOP),
      rooks = boardState.getPieces(color, chess::board::Name::ROOK),
      queens = boardState.getPieces(color, chess::board::Name::QUEEN),
      kings = boardState.getPieces(color, chess::board::Name::KING),
      allThreats = isWhite ? (chess::board::shiftNorthWest(pawns) &
                              chess::board::shiftNorthEast(pawns))
                           : (chess::board::shiftSouthWest(pawns) &
                              chess::board::shiftSouthEast(pawns)),
      empty = boardState.getEmpty(), smallestPiece = 0ULL;

  while (auto pieceInfo = chess::board::getNextPiece(knights))
    allThreats |= (pieceInfo->index > 18
                       ? chess::board::KNIGHT_SPAN << (pieceInfo->index - 18)
                       : chess::board::KNIGHT_SPAN >> (18 - pieceInfo->index)) &
                  ((pieceInfo->index % 8 < 4) ? ~chess::board::FILE_GH
                                              : ~chess::board::FILE_AB);

  while (auto pieceInfo = chess::board::getNextPiece(bishops))
    allThreats |=
        hypQuint(
            {.empty = empty,
             .square = pieceInfo->board,
             .mask = chess::board::DIAGONAL_MASKS1[pieceInfo->index % 8 +
                                                   pieceInfo->index / 8]}) |
        hypQuint(
            {.empty = empty,
             .square = pieceInfo->board,
             .mask = chess::board::DIAGONAL_MASKS2[7 - pieceInfo->index % 8 +
                                                   pieceInfo->index / 8]});

  while (auto pieceInfo = chess::board::getNextPiece(rooks))
    allThreats |=
        hypQuint({.empty = empty,
                  .square = smallestPiece,
                  .mask = chess::board::FILE_MASKS[pieceInfo->index % 8]}) |
        hypQuint({.empty = empty,
                  .square = smallestPiece,
                  .mask = chess::board::RANK_MASKS[pieceInfo->index / 8]});

  while (auto pieceInfo = chess::board::getNextPiece(queens))
    allThreats |=
        hypQuint(
            {.empty = empty,
             .square = pieceInfo->board,
             .mask = chess::board::DIAGONAL_MASKS1[pieceInfo->index % 8 +
                                                   pieceInfo->index / 8]}) |
        hypQuint(
            {.empty = empty,
             .square = pieceInfo->board,
             .mask = chess::board::DIAGONAL_MASKS2[7 - pieceInfo->index % 8 +
                                                   pieceInfo->index / 8]}) |
        hypQuint({.empty = empty,
                  .square = smallestPiece,
                  .mask = chess::board::FILE_MASKS[pieceInfo->index % 8]}) |
        hypQuint({.empty = empty,
                  .square = smallestPiece,
                  .mask = chess::board::RANK_MASKS[pieceInfo->index / 8]});

  while (auto pieceInfo = chess::board::getNextPiece(kings))
    allThreats |= (pieceInfo->index > 9
                       ? chess::board::KING_SPAN << (pieceInfo->index - 9)
                       : chess::board::KING_SPAN >> (9 - pieceInfo->index)) &
                  ((pieceInfo->index % 8 < 4) ? ~chess::board::FILE_GH
                                              : ~chess::board::FILE_AB);

  return allThreats;
}

} // namespace chess::move_generator