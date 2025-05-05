#pragma once
#include "core.hpp"
#include <array>
#include <numeric>
#include <optional>

namespace chess_board {

struct BoardState {
  struct PieceBitboards {
    std::array<Bitboard, static_cast<size_t>(Name::COUNT)> pieces = {0ULL};
  };

  std::array<PieceBitboards, static_cast<size_t>(Color::COUNT)> colorPieces_{};

  struct Castles {
    bool whiteShort = true, whiteLong = true, blackShort = true,
         blackLong = true;
  } castles_{};

  struct PieceInfo {
    Bitboard *board;
    Name name;
    Color color;
  };

  std::optional<Bitboard> enPassantSquare_;
  Color turnColor_ = Color::WHITE;

  [[nodiscard]] constexpr Bitboard getPieces(Color color, Name name) const {
    return colorPieces_[static_cast<size_t>(color)]
        .pieces[static_cast<size_t>(name)];
  }

  [[nodiscard]] constexpr Bitboard getEmpty() const {
    Bitboard occupied = 0ULL;
    for (size_t colorIndex = 0; colorIndex < 2; ++colorIndex) {
      for (size_t pieceIndex = 0; pieceIndex < static_cast<size_t>(Name::COUNT);
           ++pieceIndex) {
        occupied |= colorPieces_[colorIndex].pieces[pieceIndex];
      }
    }
    return ~occupied;
  }

  [[nodiscard]] constexpr Bitboard getPieces(Color color) const {
    Bitboard pieces = 0ULL;
    for (size_t pieceIndex = 0; pieceIndex < static_cast<size_t>(Name::COUNT);
         ++pieceIndex) {
      pieces |= colorPieces_[static_cast<size_t>(color)]
                    .pieces[static_cast<size_t>(pieceIndex)];
    }
    return pieces;
  }

  [[nodiscard]] constexpr std::optional<Bitboard> getEnPassantSquare() const {
    return enPassantSquare_;
  }

  [[nodiscard]] constexpr bool onlyKingsLeft() const {
    return ~getEmpty() == (getPieces(Color::WHITE, Name::KING) |
                           getPieces(Color::BLACK, Name::KING));
  }

  [[nodiscard]] std::optional<BoardState::PieceInfo>
  getPieceInfo(const Bitboard square) {
    for (size_t colorIndex = 0; colorIndex < 2; ++colorIndex) {
      Color color = static_cast<Color>(colorIndex);
      for (size_t nameIndex = 0; nameIndex < static_cast<size_t>(Name::COUNT);
           ++nameIndex) {
        Name name = static_cast<Name>(nameIndex);
        if (square & colorPieces_[colorIndex].pieces[nameIndex]) {
          Bitboard *actualBoardPtr =
              &colorPieces_[colorIndex].pieces[nameIndex];
          return PieceInfo{actualBoardPtr, name, color};
        }
      }
    }
    return std::nullopt;
  }

  [[nodiscard]] constexpr Color getTurnColor() const { return turnColor_; }

  [[nodiscard]] constexpr int materialScore() const {
    return 1 * std::popcount(getPieces(Color::WHITE, Name::PAWN)) +
           3 * std::popcount(getPieces(Color::WHITE, Name::KNIGHT) |
                             getPieces(Color::WHITE, Name::BISHOP)) +
           5 * std::popcount(getPieces(Color::WHITE, Name::ROOK)) +
           9 * std::popcount(getPieces(Color::WHITE, Name::QUEEN)) -
           1 * std::popcount(getPieces(Color::BLACK, Name::PAWN)) -
           3 * std::popcount(getPieces(Color::BLACK, Name::KNIGHT) |
                             getPieces(Color::BLACK, Name::BISHOP)) -
           5 * std::popcount(getPieces(Color::BLACK, Name::ROOK)) -
           9 * std::popcount(getPieces(Color::BLACK, Name::QUEEN));
  }

  [[nodiscard]] constexpr float positionScore() const {
    constexpr float PIECE_LOCATION_VALUES[8][8]{
        {-.5f, -.4f, -.4f, -.4f, -.4f, -.4f, -.4f, -.5f},
        {-.4f, -.2f, .0f, .0f, .0f, .0f, -.2f, -.4f},
        {-.4f, .0f, .1f, .2f, .2f, .1f, .0f, -.4f},
        {-.4f, .0f, .2f, .25f, .25f, .2f, .0f, -.4f},
        {-.4f, .0f, .2f, .25f, .25f, .2f, .0f, -.4f},
        {-.4f, .0f, .1f, .2f, .2f, .1f, .0f, -.4f},
        {-.4f, -.2f, .0f, .0f, .0f, .0f, -.2f, -.4f},
        {-.5f, -.4f, -.4f, -.4f, -.4f, -.4f, -.4f, -.5f}};
    constexpr float KING_LOCATION_VALUES[8][8]{
        {-.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f},
        {-.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f},
        {-.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f},
        {-.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f},
        {-.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f},
        {-.2f, -.2f, -.2f, -.2f, -.2f, -.2f, -.2f, -.2f},
        {.2f, .2f, .0f, .0f, .0f, .0f, .2f, .2f},
        {.2f, .3f, .1f, .0f, .0f, .1f, .3f, .2f}};

    auto processPieces = [&](Bitboard bb, const Color color,
                             const bool isKing) -> float {
      const float multiplier = color == Color::WHITE ? 1.0f : -1.0f;
      float localScore = 0.0f;

      while (auto pieceInfo = getNextPiece(bb)) {
        const std::uint8_t index = pieceInfo->index;

        std::uint8_t lookup_row = index / 8,
                     lookup_file =
                         (color == Color::WHITE) ? index % 8 : 7 - index % 8;

        localScore += (isKing ? KING_LOCATION_VALUES[lookup_row][lookup_file]
                              : PIECE_LOCATION_VALUES[lookup_row][lookup_file]);
      }
      return localScore * multiplier;
    };

    return processPieces(getPieces(Color::WHITE) &
                             ~getPieces(Color::WHITE, Name::KING),
                         Color::WHITE, false) +
           processPieces(getPieces(Color::BLACK) &
                             ~getPieces(Color::BLACK, Name::KING),
                         Color::BLACK, false) +
           processPieces(getPieces(Color::WHITE, Name::KING), Color::WHITE,
                         true) +
           processPieces(getPieces(Color::BLACK, Name::KING), Color::BLACK,
                         true);
  }

  // Methods for board manipulation

  bool tryMovePiece(Bitboard from, Bitboard to) {
    if (auto pieceInfo = getPieceInfo(from)) {
      *pieceInfo->board ^= (from | to);
      return true;
    }
    return false;
  }

  bool tryRemovePiece(Bitboard square) {
    if (auto pieceInfo = getPieceInfo(square)) {
      *pieceInfo->board &= ~square;
      return true;
    }
    return false;
  }

  void addPiece(Name name, Color color, Bitboard square) {
    colorPieces_[static_cast<size_t>(color)]
        .pieces[static_cast<size_t>(name)] |= square;
  }

  void setEnPassantSquare(std::optional<Bitboard> square) {
    enPassantSquare_ = square;
  }

  void updateCastlingRights(Color color, bool shortCastleLost,
                            bool longCastleLost) {
    if (color == Color::WHITE) {
      if (shortCastleLost) {
        castles_.whiteShort = false;
      }
      if (longCastleLost) {
        castles_.whiteLong = false;
      }
    } else { // color == Color::BLACK
      if (shortCastleLost) {
        castles_.blackShort = false;
      }
      if (longCastleLost) {
        castles_.blackLong = false;
      }
    }
  }

  void swapTurnColor() {
    turnColor_ = turnColor_ == Color::WHITE ? Color::BLACK : Color::WHITE;
  }
};

} // namespace chess_board