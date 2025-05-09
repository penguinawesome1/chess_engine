#pragma once
#include "masks.hpp"
#include "pieces.hpp"
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
  struct Castles {
    bool whiteShortCastle, whiteLongCastle, blackShortCastle, blackLongCastle;

    Castles()
        : whiteShortCastle(true), whiteLongCastle(true), blackShortCastle(true),
          blackLongCastle(true) {}
  };

  struct Rooks {
    chess_board::Bitboard whiteLeftRook, whiteRightRook, blackLeftRook,
        blackRightRook;

    Rooks()
        : whiteLeftRook(0ULL), whiteRightRook(0ULL), blackLeftRook(0ULL),
          blackRightRook(0ULL) {}
  };

  struct MoveBoards {
    chess_board::Bitboard start, end;

    MoveBoards() : start(0ULL), end(0ULL) {}
    MoveBoards(chess_board::Bitboard start_, chess_board::Bitboard end_)
        : start(start_), end(end_) {}
  };

  struct BoardAndName {
    std::reference_wrapper<chess_board::Bitboard> board;
    chess_board::Pieces::Name name;
  };

  struct LsbInfo {
    chess_board::Bitboard board;
    std::size_t index;
  };

  struct MoveData {
    chess_board::Pieces::Name movedName, capturedName, specialName;
    chess_board::Bitboard movedBoard, capturedBoard, specialBoard, enPassant;
    Castles castles;

    MoveData(Castles castles_, chess_board::Bitboard enPassant_)
        : movedName(chess_board::Pieces::Name::NONE),
          capturedName(chess_board::Pieces::Name::NONE),
          specialName(chess_board::Pieces::Name::NONE), movedBoard(0ULL),
          capturedBoard(0ULL), specialBoard(0ULL), castles(castles_),
          enPassant(enPassant_) {}
  };

  struct MoveGeneratorParams {
    std::vector<chess_board::Move> &moveList;
    chess_board::Color color;
    chess_board::Bitboard cantCapture, empty, pieces, threats;
    chess_board::Pieces::Name pieceName;

    MoveGeneratorParams(std::vector<chess_board::Move> &moveList_,
                        const chess_board::Color color_,
                        const chess_board::Bitboard cantCapture_,
                        const chess_board::Bitboard empty_,
                        const chess_board::Bitboard pieces_ = 0ULL,
                        const chess_board::Bitboard threats_ = 0ULL,
                        const chess_board::Pieces::Name pieceName_ =
                            chess_board::Pieces::Name::NONE)
        : moveList(moveList_), color(color_), cantCapture(cantCapture_),
          empty(empty_), pieces(pieces_), threats(threats_),
          pieceName(pieceName_) {}
  };

  struct HypQuintParams {
    chess_board::Bitboard empty, square, mask;

    HypQuintParams(const chess_board::Bitboard empty_,
                   const chess_board::Bitboard square_)
        : empty(empty_), square(square_), mask(0ULL) {}
  };

  Castles castles_{};
  Rooks rooks_{};
  std::vector<MoveData> moveHistory_;
  chess_board::Bitboard enPassant_ = 0ULL;

  static auto getMoveBoards(const chess_board::Move &localMove);
  static auto getBoardAndName(const chess_board::Bitboard square,
                              chess_board::Pieces &localPieces)
      -> std::optional<BoardAndName>;
  static auto getBoardsAndNamePromotion(const chess_board::Move::Type localType,
                                        chess_board::Pieces &pieces)
      -> std::optional<std::tuple<std::reference_wrapper<chess_board::Bitboard>,
                                  std::reference_wrapper<chess_board::Bitboard>,
                                  chess_board::Pieces::Name>>;
  auto getBoardAndNameCastle(const chess_board::Move::Type localType,
                             chess_board::Pieces &pieces)
      -> std::optional<
          std::tuple<std::reference_wrapper<chess_board::Bitboard>,
                     chess_board::Bitboard, chess_board::Pieces::Name>>;
  void handleCapture(const chess_board::Bitboard endBoard, MoveData &moveData,
                     chess_board::Pieces &pieces);
  void handleMovedPiece(const MoveBoards &moveBoards, MoveData &moveData,
                        chess_board::Pieces &pieces);
  void handlePromotion(const chess_board::Move::Type localType,
                       const chess_board::Bitboard endBoard, MoveData &moveData,
                       chess_board::Pieces &pieces);
  void handleCastling(const chess_board::Move::Type localType,
                      MoveData &moveData, chess_board::Pieces &pieces);
  void handleEnPassant(chess_board::Pieces &pieces,
                       const MoveBoards &moveBoards,
                       const chess_board::Move::Type moveType,
                       MoveData &moveData);
  static constexpr chess_board::Bitboard hypQuint(const HypQuintParams &params);
  static constexpr chess_board::Bitboard reverse(chess_board::Bitboard b);
  static std::optional<Moves::LsbInfo>
  getNextPiece(chess_board::Bitboard &board);
  static chess_board::Cords getCordsFromIndex(std::size_t index);

public:
  Moves() = default;
  void initRooks(const chess_board::Pieces &pieces);
  void doMove(const chess_board::Move &move, chess_board::Pieces &pieces);
  void undoMove(chess_board::Pieces &pieces);
  std::vector<chess_board::Move>
  getPossibleMoves(const chess_board::Color color,
                   const chess_board::Pieces &pieces) const;
  chess_board::Bitboard
  getThreatSquares(const chess_board::Color color,
                   const chess_board::Pieces &pieces) const;
}; // class Moves

} // namespace chess_moves