#pragma once
#include <cstdint>
#include <iostream>
#include <string_view>

namespace chess_board {

using Bitboard = uint64_t;

struct Pieces {
  Bitboard whitePawns;
  Bitboard whiteKnights;
  Bitboard whiteBishops;
  Bitboard whiteRooks;
  Bitboard whiteQueens;
  Bitboard whiteKings;

  Bitboard blackPawns;
  Bitboard blackKnights;
  Bitboard blackBishops;
  Bitboard blackRooks;
  Bitboard blackQueens;
  Bitboard blackKings;

  Bitboard enPassant;
};

enum class GameType { CHESS, CHESS960 };
enum class OpponentType { HUMAN, ENGINE };
enum class Color { WHITE, BLACK };

struct GameParams {
  GameType gameType;
  OpponentType opponentType;
  Color playerColor;
  int depth;
};

struct Cords {
  std::size_t x;
  std::size_t y;

  Cords() : x(0), y(0) {}
  Cords(std::size_t _x, std::size_t _y) : x(_x), y(_y) {}

  bool operator==(const Cords &other) const {
    return x == other.x && y == other.y;
  }
};

struct Move {
  enum class Type {
    NORMAL,
    CAPTURE,
    EN_PASSANT,
    WHITE_CASTLE_KINGSIDE,
    WHITE_CASTLE_QUEENSIDE,
    BLACK_CASTLE_KINGSIDE,
    BLACK_CASTLE_QUEENSIDE,
    WHITE_PAWN_PROMOTION_KNIGHT,
    WHITE_PAWN_PROMOTION_BISHOP,
    WHITE_PAWN_PROMOTION_ROOK,
    WHITE_PAWN_PROMOTION_QUEEN,
    BLACK_PAWN_PROMOTION_KNIGHT,
    BLACK_PAWN_PROMOTION_BISHOP,
    BLACK_PAWN_PROMOTION_ROOK,
    BLACK_PAWN_PROMOTION_QUEEN
  };

  Cords startSquare;
  Cords endSquare;
  Type type;

  Move(Cords startSquare_, Cords endSquare_, Type type_)
      : startSquare(startSquare_), endSquare(endSquare_), type(type_) {}

  Move(Cords startSquare_, Cords endSquare_)
      : startSquare(startSquare_), endSquare(endSquare_), type(Type::NORMAL) {}

  Move() : startSquare(Cords{}), endSquare(Cords{}), type(Type::NORMAL) {}

  bool operator==(const Move &other) const {
    return startSquare == other.startSquare && endSquare == other.endSquare &&
           type == other.type;
  }

  const char *getSpecialMoveName() const {
    constexpr std::string_view specialMoveNames[] = {
        "NORMAL",
        "WHITE_CASTLE_KINGSIDE",
        "WHITE_CASTLE_QUEENSIDE",
        "BLACK_CASTLE_KINGSIDE",
        "BLACK_CASTLE_QUEENSIDE",
        "WHITE_PAWN_PROMOTION_KNIGHT",
        "WHITE_PAWN_PROMOTION_BISHOP",
        "WHITE_PAWN_PROMOTION_ROOK",
        "WHITE_PAWN_PROMOTION_QUEEN",
        "BLACK_PAWN_PROMOTION_KNIGHT",
        "BLACK_PAWN_PROMOTION_BISHOP",
        "BLACK_PAWN_PROMOTION_ROOK",
        "BLACK_PAWN_PROMOTION_QUEEN",
        "EN_PASSANT"};
    return specialMoveNames[static_cast<std::size_t>(type)].data();
  }

  void print() const {
    std::cout << "(" << startSquare.x << ", " << startSquare.y << ") -> ("
              << endSquare.x << ", " << endSquare.y
              << ") Type: " << getSpecialMoveName() << "\n";
  }
};

} // namespace chess_board