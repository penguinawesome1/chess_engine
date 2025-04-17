#pragma once
#include <cstdint>
#include <iostream>

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
  enum class MoveType {
    NORMAL,
    CASTLE_KINGSIDE,
    CASTLE_QUEENSIDE,
    EN_PASSANT,
    PAWN_PROMOTION_KNIGHT,
    PAWN_PROMOTION_BISHOP,
    PAWN_PROMOTION_ROOK,
    PAWN_PROMOTION_QUEEN
  };

  Cords startSquare;
  Cords endSquare;
  MoveType moveType;

  Move(Cords startSquare_, Cords endSquare_)
      : startSquare(startSquare_), endSquare(endSquare_),
        moveType(MoveType::NORMAL) {}

  Move()
      : startSquare(Cords{}), endSquare(Cords{}), moveType(MoveType::NORMAL) {}

  bool operator==(const Move &other) const {
    return startSquare == other.startSquare && endSquare == other.endSquare &&
           moveType == other.moveType;
  }

  void print() const {
    std::cout << "(" << startSquare.x << ", " << startSquare.y << ") -> ("
              << endSquare.x << ", " << endSquare.y << ") Type: ";
    switch (moveType) {
    case MoveType::NORMAL:
      std::cout << "NORMAL";
      break;
    case MoveType::CASTLE_KINGSIDE:
      std::cout << "CASTLE_KINGSIDE";
      break;
    case MoveType::CASTLE_QUEENSIDE:
      std::cout << "CASTLE_QUEENSIDE";
      break;
    case MoveType::EN_PASSANT:
      std::cout << "EN_PASSANT";
      break;
    case MoveType::PAWN_PROMOTION_KNIGHT:
      std::cout << "PAWN_PROMOTION_KNIGHT";
      break;
    case MoveType::PAWN_PROMOTION_BISHOP:
      std::cout << "PAWN_PROMOTION_BISHOP";
      break;
    case MoveType::PAWN_PROMOTION_ROOK:
      std::cout << "PAWN_PROMOTION_ROOK";
      break;
    case MoveType::PAWN_PROMOTION_QUEEN:
      std::cout << "PAWN_PROMOTION_QUEEN";
      break;
    }
    std::cout << "\n";
  }
};

} // namespace chess_board