#include "move_generator.hpp"

namespace chess_move_generator {

std::vector<chess_board::Move>
getPossibleMoves(const chess_board::BoardState &boardState) {
  std::vector<chess_board::Move> moveList;
  moveList.reserve(256); // Reserve capacity to avoid reallocations.

  // const chess_board::Color color = boardState.getTurnColor(),
  //                          otherColor = getOppositeColor(color);
  // const bool isWhite = color == chess_board::Color::WHITE;

  //// --- Initialize Generators ---
  //// Pass boardState struct for common parameters.
  //// Pass moveList by reference for output.

  // PawnGenerator pawnGenerator(
  //     {.moveList = moveList,
  //      .boardState = boardState,
  //      .pawns = boardState.getPieces(color, chess_board::Name::PAWN),
  //      .promotionRank = isWhite ? chess_board::constants::RANK_8
  //                               : chess_board::constants::RANK_1,
  //      .pawnStartRank = isWhite ? chess_board::constants::RANK_2
  //                               : chess_board::constants::RANK_7,
  //      .enPassantTargetSquare =
  //          boardState.getEnPassantSquare().value_or(0ULL)});

  // KnightGenerator knightGenerator(
  //     {.moveList = moveList,
  //      .boardState = boardState,
  //      .knights = isWhite ? boardState.getPieces(color,
  //      chess_board::Name::KNIGHT)});

  // SliderGenerator sliderGenerator(
  //     {.moveList = moveList,
  //      .boardState = boardState,
  //      .bishops = boardState.getPieces(color, chess_board::Name::BISHOP),
  //      .rooks = boardState.getPieces(color, chess_board::Name::ROOK),
  //      .queens = boardState.getPieces(color, chess_board::Name::QUEEN)});

  // KingGenerator kingGenerator(
  //     {.moveList = moveList,
  //      .boardState = boardState,
  //      .kings = boardState.getPieces(color, chess_board::Name::KING),
  //      .threats = getThreatSquares(color, boardState),
  //      .castles = boardState.castles_});

  //// --- Generate Moves ---
  //// Prioritize better moves first for alpha beta pruning.

  // sliderGenerator.addDiagonalTargets(true);    // queens
  // sliderGenerator.addOrthogonalTargets(true);  // queens
  // sliderGenerator.addDiagonalTargets(false);   // bishops
  // sliderGenerator.addOrthogonalTargets(false); // rooks
  // knightGenerator.addKnightTargets();          // knights
  // pawnGenerator.addLeftCaptureTargets();       // pawns
  // pawnGenerator.addRightCaptureTargets();      // pawns
  // pawnGenerator.addPushTwiceTargets();         // pawns
  // pawnGenerator.addPushOnceTargets();          // pawns
  // kingGenerator.addKingTargets();              // kings
  // kingGenerator.addCastleTargets();            // kings

  return moveList;
}

chess_board::Bitboard
getThreatSquares(const chess_board::Color color,
                 const chess_board::BoardState &boardState) {
  return 0ULL; ////////////////

  /*const bool isWhite = (color == chess_board::Color::WHITE);

  chess_board::Bitboard
      pawns = isWhite ? pieces.whitePawns : pieces.blackPawns,
      knights = isWhite ? pieces.whiteKnights : pieces.blackKnights,
      bishops = isWhite ? pieces.whiteBishops : pieces.blackBishops,
      rooks = isWhite ? pieces.whiteRooks : pieces.blackRooks,
      queens = isWhite ? pieces.whiteQueens : pieces.blackQueens,
      kings = isWhite ? pieces.whiteKings : pieces.blackKings,
      allThreats = isWhite ? (pawns >> 7 & ~chess_board::constants::FILE_A) |
                                 (pawns >> 9 &
  ~chess_board::constants::FILE_H) : (pawns << 7 &
  ~chess_board::constants::FILE_H) | (pawns << 9 &
  ~chess_board::constants::FILE_A), empty = pieces.getEmpty();

  std::uint8_t pieceIndex;

  chess_board::Bitboard tempKnights = knights;
  while (auto knightInfo = getNextPiece(tempKnights)) {
    pieceIndex = knightInfo->index;
    chess_board::Bitboard possibilities =
        (pieceIndex > 18
             ? chess_board::constants::KNIGHT_SPAN << (pieceIndex - 18)
             : chess_board::constants::KNIGHT_SPAN >> (18 - pieceIndex)) &
        ((pieceIndex % 8 < 4) ? ~chess_board::constants::FILE_GH
                              : ~chess_board::constants::FILE_AB);
    allThreats |= possibilities;
  }

  while (auto bishopInfo = getNextPiece(bishops)) {
    pieceIndex = bishopInfo->index;

    HypQuintParams params{empty, bishopInfo->board};
    params.mask = chess_board::constants::DIAGONAL_MASKS1[pieceIndex / 8 +
                                                          pieceIndex % 8];
    allThreats |= hypQuint(params);

    params.mask = chess_board::constants::DIAGONAL_MASKS2[pieceIndex / 8 + 7 -
                                                          pieceIndex % 8];
    allThreats |= hypQuint(params);
  }

  while (auto rookInfo = getNextPiece(rooks)) {
    pieceIndex = rookInfo->index;

    HypQuintParams params{empty, rookInfo->board};
    params.mask = chess_board::constants::FILE_MASKS[pieceIndex % 8];
    allThreats |= hypQuint(params);

    params.mask = chess_board::constants::RANK_MASKS[pieceIndex / 8];
    allThreats |= hypQuint(params);
  }

  while (auto queenInfo = getNextPiece(queens)) {
    pieceIndex = queenInfo->index;

    HypQuintParams params{empty, queenInfo->board};
    params.mask = chess_board::constants::DIAGONAL_MASKS1[pieceIndex / 8 +
                                                          pieceIndex % 8];
    allThreats |= hypQuint(params);

    params.mask = chess_board::constants::DIAGONAL_MASKS2[pieceIndex / 8 + 7 -
                                                          pieceIndex % 8];
    allThreats |= hypQuint(params);

    params.mask = chess_board::constants::FILE_MASKS[pieceIndex % 8];
    allThreats |= hypQuint(params);

    params.mask = chess_board::constants::RANK_MASKS[pieceIndex / 8];
    allThreats |= hypQuint(params);
  }

  while (auto kingInfo = getNextPiece(kings)) {
    std::uint8_t pieceIndex = kingInfo->index;
    chess_board::Bitboard smallestPiece = kingInfo->board;

    allThreats |=
        (pieceIndex > 9
             ? chess_board::constants::KING_SPAN << (pieceIndex - 9)
             : chess_board::constants::KING_SPAN >> (9 - pieceIndex)) &
        ((pieceIndex % 8 < 4) ? ~chess_board::constants::FILE_GH
                              : ~chess_board::constants::FILE_AB);
  }

  return allThreats;*/
}

} // namespace chess_move_generator