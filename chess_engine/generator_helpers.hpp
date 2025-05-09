// #pragma once
// #include "board_state.hpp"
// #include "constants.hpp"
// #include "core.hpp"
// #include "move_generator.hpp"
// #include <bit>
// #include <optional>
// #include <vector>
//
// namespace chess_move_generator {
//
// struct PawnGenerator {
//   struct Params {
//     std::vector<chess_board::Move> &moveList;
//     chess_board::BoardState &boardState;
//     chess_board::Bitboard pawns = 0ULL, promotionRank = 0ULL,
//                           pawnStartRank = 0ULL, enPassantTargetSquare = 0ULL;
//   };
//
//   std::vector<chess_board::Move> &moveList;
//   chess_board::Color color;
//   chess_board::Bitboard pawns, empty, occupied, friendlyPieces,
//   promotionRank,
//       pawnStartRank, enPassantTargetSquare;
//
//   PawnGenerator(const Params &params);
//   void addPushOnceTargets() const;
//   void addPushTwiceTargets() const;
//   void addLeftCaptureTargets() const;
//   void addRightCaptureTargets() const;
//   void addNormalMove(
//       chess_board::Bitboard pieces, int offset,
//       chess_board::Move::Type type = chess_board::Move::Type::NORMAL) const;
//   void addPromotionMove(chess_board::Bitboard pieces, int offset) const;
//   bool isWhite() const;
// };
//
// struct KnightGenerator {
//   struct Params {
//     std::vector<chess_board::Move> &moveList;
//     chess_board::BoardState boardState;
//     chess_board::Bitboard knights = 0ULL;
//   };
//
//   std::vector<chess_board::Move> &moveList;
//   chess_board::Color color;
//   chess_board::Bitboard knights, empty, occupied, friendlyPieces;
//
//   KnightGenerator(const Params &params);
//   void addKnightTargets() const;
// };
//
// struct SliderGenerator {
//   struct Params {
//     std::vector<chess_board::Move> &moveList;
//     chess_board::BoardState boardState;
//     chess_board::Bitboard bishops = 0ULL, rooks = 0ULL, queens = 0ULL;
//   };
//
//   std::vector<chess_board::Move> &moveList;
//   chess_board::Color color;
//   chess_board::Bitboard bishops, rooks, queens, empty, occupied,
//   friendlyPieces;
//
//   SliderGenerator(const Params &params);
//   void addMoves(std::uint8_t startIndex,
//                 chess_board::Bitboard possibilities) const;
//   void addDiagonalTargets(bool isQueen) const;
//   void addOrthogonalTargets(bool isQueen) const;
// };
//
// struct KingGenerator {
//   struct Params {
//     std::vector<chess_board::Move> &moveList;
//     chess_board::BoardState boardState;
//     chess_board::Bitboard kings = 0ULL, threats = 0ULL;
//     chess_board::BoardState::Castles castles;
//   };
//
//   std::vector<chess_board::Move> &moveList;
//   chess_board::Color color;
//   chess_board::Bitboard kings, empty, occupied, friendlyPieces, threats;
//   chess_board::BoardState::Castles castles;
//
//   KingGenerator(const Params &params);
//   void addMoves(
//       std::uint8_t startIndex, chess_board::Bitboard possibilities,
//       chess_board::Move::Type type = chess_board::Move::Type::NORMAL) const;
//   void addKingTargets() const;
//   void addCastleTargets() const;
// };
//
// } // namespace chess_move_generator