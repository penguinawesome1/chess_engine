module;

#include <vector>

module chess.move_generator;

import chess.core;
import chess.board_state;
import chess.move;
import chess.masks;
import chess.generator_helpers;

namespace chess::move_generator {

auto getPossibleMoves(const chess::board::BoardState &boardState)
    -> std::vector<chess::board::Move> {
  std::vector<chess::board::Move> moveList;
  moveList.reserve(256); // Reserve capacity to avoid reallocations.

  const chess::board::Color color = boardState.getTurnColor(),
                            otherColor = getOppositeColor(color);
  const bool isWhite = color == chess::board::Color::WHITE;

  // --- Initialize Generators ---
  // Pass boardState struct for common parameters.
  // Pass moveList by reference for output.

  using chess::board::Name;

  PawnGenerator pawnGenerator(
      {.moveList = moveList,
       .boardState = boardState,
       .pawns = boardState.getPieces(color, Name::PAWN),
       .promotionRank = (isWhite ? chess::board::RANK_8 : chess::board::RANK_1),
       .pawnStartRank = (isWhite ? chess::board::RANK_2 : chess::board::RANK_7),
       .enPassantTargetSquare =
           boardState.getEnPassantSquare().value_or(0ULL)});

  KnightGenerator knightGenerator(
      {.moveList = moveList,
       .boardState = boardState,
       .knights = boardState.getPieces(color, Name::KNIGHT)});

  SliderGenerator sliderGenerator(
      {.moveList = moveList,
       .boardState = boardState,
       .bishops = boardState.getPieces(color, Name::BISHOP),
       .rooks = boardState.getPieces(color, Name::ROOK),
       .queens = boardState.getPieces(color, Name::QUEEN)});

  KingGenerator kingGenerator({.moveList = moveList,
                               .boardState = boardState,
                               .kings = boardState.getPieces(color, Name::KING),
                               .threats = getThreatSquares(color, boardState),
                               .castles = boardState.castles_});

  // --- Generate Moves ---
  // Prioritize better moves first for alpha beta pruning.

  sliderGenerator.addDiagonalTargets(true);    // queens
  sliderGenerator.addOrthogonalTargets(true);  // queens
  sliderGenerator.addDiagonalTargets(false);   // bishops
  sliderGenerator.addOrthogonalTargets(false); // rooks
  knightGenerator.addKnightTargets();          // knights
  pawnGenerator.addWestCaptureTargets();       // pawns
  pawnGenerator.addEastCaptureTargets();       // pawns
  pawnGenerator.addPushTwiceTargets();         // pawns
  pawnGenerator.addPushOnceTargets();          // pawns
  kingGenerator.addKingTargets();              // kings
  kingGenerator.addCastleTargets();            // kings

  return moveList;
}

} // namespace chess::move_generator