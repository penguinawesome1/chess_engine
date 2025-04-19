#include "Moves.hpp"

namespace chess_moves {

void Moves::initRooks(const chess_board::Pieces &pieces) {
  rooks_.whiteLeftRook = pieces.whiteRooks & (~pieces.whiteRooks + 1);
  rooks_.whiteRightRook = pieces.whiteRooks ^ rooks_.whiteLeftRook;
  rooks_.blackLeftRook = pieces.blackRooks & (~pieces.blackRooks + 1);
  rooks_.blackRightRook = pieces.blackRooks ^ rooks_.blackLeftRook;
}

auto Moves::getMoveBoards(const chess_board::Move &localMove) {
  const chess_board::Bitboard start =
      1ULL << (localMove.startSquare.x + 8 * (7 - localMove.startSquare.y));
  const chess_board::Bitboard end =
      1ULL << (localMove.endSquare.x + 8 * (7 - localMove.endSquare.y));
  return MoveBoards(start, end);
};

auto Moves::getBoardAndName(chess_board::Bitboard square,
                            chess_board::Pieces &pieces)
    -> std::optional<
        std::pair<std::reference_wrapper<chess_board::Bitboard>, PieceName>> {
  using BoardNamePair =
      std::pair<std::reference_wrapper<chess_board::Bitboard>, PieceName>;
  std::array<BoardNamePair, 12> pieceMappings = {
      BoardNamePair{std::ref(pieces.whitePawns), PieceName::WHITE_PAWNS},
      BoardNamePair{std::ref(pieces.whiteKnights), PieceName::WHITE_KNIGHTS},
      BoardNamePair{std::ref(pieces.whiteBishops), PieceName::WHITE_BISHOPS},
      BoardNamePair{std::ref(pieces.whiteRooks), PieceName::WHITE_ROOKS},
      BoardNamePair{std::ref(pieces.whiteQueens), PieceName::WHITE_QUEENS},
      BoardNamePair{std::ref(pieces.whiteKings), PieceName::WHITE_KINGS},
      BoardNamePair{std::ref(pieces.blackPawns), PieceName::BLACK_PAWNS},
      BoardNamePair{std::ref(pieces.blackKnights), PieceName::BLACK_KNIGHTS},
      BoardNamePair{std::ref(pieces.blackBishops), PieceName::BLACK_BISHOPS},
      BoardNamePair{std::ref(pieces.blackRooks), PieceName::BLACK_ROOKS},
      BoardNamePair{std::ref(pieces.blackQueens), PieceName::BLACK_QUEENS},
      BoardNamePair{std::ref(pieces.blackKings), PieceName::BLACK_KINGS}};

  for (const auto &mapping : pieceMappings) {
    if (square & mapping.first.get()) {
      return std::make_pair(mapping.first, mapping.second);
    }
  }

  return std::nullopt;
};

auto Moves::getBoardsAndNamePromotion(chess_board::Move::Type localType,
                                      chess_board::Pieces &pieces)
    -> std::optional<
        std::tuple<std::reference_wrapper<chess_board::Bitboard>,
                   std::reference_wrapper<chess_board::Bitboard>, PieceName>> {
  using PromotionInfo =
      std::tuple<std::reference_wrapper<chess_board::Bitboard>,
                 std::reference_wrapper<chess_board::Bitboard>, PieceName>;
  static const std::map<chess_board::Move::Type, PromotionInfo>
      promotionMappings = {
          {chess_board::Move::Type::WHITE_PAWN_PROMOTION_KNIGHT,
           {std::ref(pieces.whiteKnights), std::ref(pieces.whitePawns),
            PieceName::WHITE_KNIGHTS}},
          {chess_board::Move::Type::WHITE_PAWN_PROMOTION_BISHOP,
           {std::ref(pieces.whiteBishops), std::ref(pieces.whitePawns),
            PieceName::WHITE_BISHOPS}},
          {chess_board::Move::Type::WHITE_PAWN_PROMOTION_ROOK,
           {std::ref(pieces.whiteRooks), std::ref(pieces.whitePawns),
            PieceName::WHITE_ROOKS}},
          {chess_board::Move::Type::WHITE_PAWN_PROMOTION_QUEEN,
           {std::ref(pieces.whiteQueens), std::ref(pieces.whitePawns),
            PieceName::WHITE_QUEENS}},
          {chess_board::Move::Type::BLACK_PAWN_PROMOTION_KNIGHT,
           {std::ref(pieces.blackKnights), std::ref(pieces.blackPawns),
            PieceName::BLACK_KNIGHTS}},
          {chess_board::Move::Type::BLACK_PAWN_PROMOTION_BISHOP,
           {std::ref(pieces.blackBishops), std::ref(pieces.blackPawns),
            PieceName::BLACK_BISHOPS}},
          {chess_board::Move::Type::BLACK_PAWN_PROMOTION_ROOK,
           {std::ref(pieces.blackRooks), std::ref(pieces.blackPawns),
            PieceName::BLACK_ROOKS}},
          {chess_board::Move::Type::BLACK_PAWN_PROMOTION_QUEEN,
           {std::ref(pieces.blackQueens), std::ref(pieces.blackPawns),
            PieceName::BLACK_QUEENS}}};
  auto it = promotionMappings.find(localType);
  if (it != promotionMappings.end()) {
    return it->second;
  } else {
    return std::nullopt;
  }
}

auto Moves::getBoardAndNameCastle(chess_board::Move::Type localType,
                                  chess_board::Pieces &pieces)
    -> std::optional<std::tuple<std::reference_wrapper<chess_board::Bitboard>,
                                chess_board::Bitboard, PieceName>> {
  using CastleInfo = std::tuple<std::reference_wrapper<chess_board::Bitboard>,
                                chess_board::Bitboard, PieceName>;
  static const std::map<chess_board::Move::Type, CastleInfo> castleMappings = {
      {chess_board::Move::Type::WHITE_CASTLE_KINGSIDE,
       {std::ref(pieces.whiteRooks), rooks_.whiteRightRook,
        PieceName::WHITE_ROOKS}},
      {chess_board::Move::Type::WHITE_CASTLE_QUEENSIDE,
       {std::ref(pieces.whiteRooks), rooks_.whiteLeftRook,
        PieceName::WHITE_ROOKS}},
      {chess_board::Move::Type::BLACK_CASTLE_KINGSIDE,
       {std::ref(pieces.blackRooks), rooks_.blackRightRook,
        PieceName::BLACK_ROOKS}},
      {chess_board::Move::Type::BLACK_CASTLE_QUEENSIDE,
       {std::ref(pieces.blackRooks), rooks_.blackLeftRook,
        PieceName::BLACK_ROOKS}}};
  auto it = castleMappings.find(localType);
  if (it != castleMappings.end()) {
    return it->second;
  } else {
    return std::nullopt;
  }
};

void Moves::handleCapture(chess_board::Bitboard endSquare, MoveData moveData,
                          chess_board::Pieces &pieces) {
  auto result = getBoardAndName(endSquare, pieces);
  if (result.has_value()) {
    auto &[boardRef, name] = result.value();
    moveData.capturedName = name;
    moveData.capturedBoard = boardRef;
    boardRef ^= endSquare;
    if (name == PieceName::WHITE_ROOKS) {
      castles_.whiteShortCastle &= endSquare != rooks_.whiteRightRook;
      castles_.whiteLongCastle &= endSquare != rooks_.whiteLeftRook;
    } else if (name == PieceName::BLACK_ROOKS) {
      castles_.blackShortCastle &= endSquare != rooks_.blackRightRook;
      castles_.blackLongCastle &= endSquare != rooks_.blackLeftRook;
    }
  }
};

void Moves::handleMovedPiece(MoveBoards moveBoards, MoveData moveData,
                             chess_board::Pieces pieces) {
  auto result = getBoardAndName(moveBoards.start, pieces);
  if (result.has_value()) {
    auto &[boardRef, name] = result.value();
    moveData.movedName = name;
    moveData.movedBoard = boardRef;
    boardRef ^= moveBoards.start | moveBoards.end;
    if (name == PieceName::WHITE_ROOKS) {
      castles_.whiteShortCastle &= moveBoards.end != rooks_.whiteRightRook;
      castles_.whiteLongCastle &= moveBoards.end != rooks_.whiteLeftRook;
    } else if (name == PieceName::BLACK_ROOKS) {
      castles_.blackShortCastle &= moveBoards.end != rooks_.blackRightRook;
      castles_.blackLongCastle &= moveBoards.end != rooks_.blackLeftRook;
    } else if (name == PieceName::WHITE_KINGS) {
      castles_.whiteShortCastle = false;
      castles_.whiteLongCastle = false;
    } else if (name == PieceName::BLACK_KINGS) {
      castles_.blackShortCastle = false;
      castles_.blackLongCastle = false;
    }
  }
};

void Moves::handlePromotion(chess_board::Move::Type localType,
                            chess_board::Bitboard endSquare, MoveData moveData,
                            chess_board::Pieces pieces) {
  auto result = getBoardsAndNamePromotion(localType, pieces);
  if (result.has_value()) {
    auto &[boardRef, pawnBoardRef, name] = result.value();
    moveData.specialName = name;
    moveData.specialBoard = boardRef;
    pawnBoardRef ^= endSquare;
    boardRef |= endSquare;
  }
};

void Moves::handleCastling(chess_board::Move::Type localType, MoveData moveData,
                           chess_board::Pieces pieces) {
  auto result = getBoardAndNameCastle(localType, pieces);
  if (result.has_value()) {
    auto &[boardRef, rooksBoard, name] = result.value();
    moveData.specialName = name;
    moveData.specialBoard = boardRef;
    boardRef ^= rooksBoard;
  }
};

void Moves::doMove(const chess_board::Move &move, chess_board::Pieces &pieces) {
  using namespace chess_board;

  const MoveBoards moveBoards = getMoveBoards(move);

  MoveData moveData{castles_};

  const Bitboard enPassantTemp = pieces.enPassant;
  const bool isDoublePawnPush =
      (pieces.whitePawns | pieces.blackPawns) & moveBoards.start &&
      (moveBoards.start << 16 | moveBoards.start >> 16) & moveBoards.end;
  pieces.enPassant = isDoublePawnPush ? moveBoards.end : 0;

  if (move.type == Move::Type::EN_PASSANT) {
    if (pieces.whitePawns & moveBoards.start) {
      moveData.capturedBoard = pieces.whitePawns;
      pieces.whitePawns ^= enPassantTemp;
    } else {
      moveData.capturedBoard = pieces.blackPawns;
      pieces.blackPawns ^= enPassantTemp;
    }
  }

  handleCapture(moveBoards.end, moveData, pieces);
  handleMovedPiece(moveBoards, moveData, pieces);
  handlePromotion(move.type, moveBoards.end, moveData, pieces);
  handleCastling(move.type, moveData, pieces);

  moveHistory_.push_back(moveData);
}

void Moves::undoMove(chess_board::Pieces pieces) {
  const MoveData moveData = moveHistory_.back();
  const std::vector<PieceName> names = {
      moveData.capturedName, moveData.movedName, moveData.specialName};
  const std::vector<chess_board::Bitboard> boards = {
      moveData.capturedBoard, moveData.movedBoard, moveData.specialBoard};
  castles_ = moveData.castles;
  moveHistory_.pop_back();

  std::map<PieceName, uint64_t *> pieceMap = {
      {PieceName::WHITE_PAWNS, &pieces.whitePawns},
      {PieceName::WHITE_KNIGHTS, &pieces.whiteKnights},
      {PieceName::WHITE_BISHOPS, &pieces.whiteBishops},
      {PieceName::WHITE_ROOKS, &pieces.whiteRooks},
      {PieceName::WHITE_QUEENS, &pieces.whiteQueens},
      {PieceName::WHITE_KINGS, &pieces.whiteKings},
      {PieceName::BLACK_PAWNS, &pieces.blackPawns},
      {PieceName::BLACK_KNIGHTS, &pieces.blackKnights},
      {PieceName::BLACK_BISHOPS, &pieces.blackBishops},
      {PieceName::BLACK_ROOKS, &pieces.blackRooks},
      {PieceName::BLACK_QUEENS, &pieces.blackQueens},
      {PieceName::BLACK_KINGS, &pieces.blackKings}};

  for (int i = 0; i < 3; ++i) {
    if (pieceMap.count(names[i]))
      *pieceMap[names[i]] = boards[i];
  }
}

std::vector<chess_board::Move>
Moves::getPossibleMoves(chess_board::Color color,
                        chess_board::Pieces pieces) const {
  std::vector<chess_board::Move> allMoves;

  const chess_board::Bitboard cantCapture =
      color == chess_board::Color::WHITE
          ? (pieces.whitePawns | pieces.whiteKnights | pieces.whiteBishops |
             pieces.whiteRooks | pieces.whiteQueens | pieces.whiteKings)
          : (pieces.blackPawns | pieces.blackKnights | pieces.blackBishops |
             pieces.blackRooks | pieces.blackQueens | pieces.blackKings);
  const chess_board::Bitboard occupied =
      pieces.whitePawns | pieces.whiteKnights | pieces.whiteBishops |
      pieces.whiteRooks | pieces.whiteQueens | pieces.whiteKings |
      pieces.blackPawns | pieces.blackKnights | pieces.blackBishops |
      pieces.blackRooks | pieces.blackQueens | pieces.blackKings;
  const chess_board::Bitboard threats = getThreatSquares(color, pieces);

  const bool isWhite = color == chess_board::Color::WHITE;

  possibleP(allMoves, color, cantCapture, occupied, pieces.enPassant,
            isWhite ? pieces.whitePawns : pieces.blackPawns);
  possibleN(allMoves, color, cantCapture,
            isWhite ? pieces.whiteKnights : pieces.blackKnights);
  possibleSliderMoves(allMoves, color, PieceName::WHITE_BISHOPS, cantCapture,
                      occupied,
                      isWhite ? pieces.whiteBishops : pieces.blackBishops);
  possibleSliderMoves(allMoves, color, PieceName::WHITE_ROOKS, cantCapture,
                      occupied,
                      isWhite ? pieces.whiteRooks : pieces.blackRooks);
  possibleSliderMoves(allMoves, color, PieceName::WHITE_QUEENS, cantCapture,
                      occupied,
                      isWhite ? pieces.whiteQueens : pieces.blackQueens);
  possibleK(allMoves, color, cantCapture, occupied, threats,
            isWhite ? pieces.whiteKings : pieces.blackKings);

  return allMoves;
}

void Moves::possibleP(std::vector<chess_board::Move> &allMoves,
                      const chess_board::Color color,
                      chess_board::Bitboard cantCapture,
                      chess_board::Bitboard empty,
                      chess_board::Bitboard enPassant,
                      chess_board::Bitboard pawns) const {
  using namespace chess_board;

  const int forward = color == Color::WHITE ? -1 : 1;
  const Bitboard startRankDoublePush =
      color == Color::WHITE ? masks_.RANK_2 : masks_.RANK_7;
  const Bitboard promotionRank =
      color == Color::WHITE ? masks_.RANK_7 : masks_.RANK_2;
  const Bitboard notFileA = ~masks_.FILE_A;
  const Bitboard notFileH = ~masks_.FILE_H;

  auto addMove = [&](Bitboard targets, int colOffset, Move::Type type) {
    Bitboard currentTargets = targets;
    while (currentTargets) {
      const int endSquareIndex = std::countr_zero(currentTargets);
      const int endRow = endSquareIndex % 8;
      const int endCol = endSquareIndex / 8;
      const int startRow = endRow - forward;
      const int startCol = endCol - colOffset;
      allMoves.push_back(
          {Cords(startCol, startRow), Cords(endCol, endRow), type});
      currentTargets &= currentTargets - 1;
    }
  };

  auto addPromotionMove = [&](Bitboard targets, int colOffset) {
    Bitboard currentTargets = targets;
    while (currentTargets) {
      const int endSquareIndex = std::countr_zero(currentTargets);
      const int endRow = endSquareIndex % 8;
      const int endCol = endSquareIndex / 8;
      const int startRow = endRow - forward;
      const int startCol = endCol - colOffset;
      const Cords start(startCol, startRow);
      const Cords end(endCol, endRow);
      const auto basePromotionType =
          color == Color::WHITE ? Move::Type::WHITE_PAWN_PROMOTION_KNIGHT
                                : Move::Type::BLACK_PAWN_PROMOTION_KNIGHT;
      for (int i = 0; i < 4; ++i) {
        allMoves.push_back(
            {start, end,
             static_cast<Move::Type>(static_cast<int>(basePromotionType) + i)});
      }
      currentTargets &= currentTargets - 1;
    }
  };

  const Bitboard pushOnceTargets =
      (color == Color::WHITE ? (pawns << 8) : (pawns >> 8)) & empty &
      ~promotionRank;
  addMove(pushOnceTargets, 0, Move::Type::NORMAL);

  const Bitboard pushTwiceTargets =
      (color == Color::WHITE ? (pawns << 16) : (pawns >> 16)) & empty &
      (color == Color::WHITE ? (empty << 8) : (empty >> 8)) &
      startRankDoublePush;
  addMove(pushTwiceTargets, 0, Move::Type::NORMAL);

  const Bitboard leftCaptureTargets =
      (color == Color::WHITE ? (pawns << 7) : (pawns >> 9)) &
      (~empty | enPassant) & ~cantCapture & notFileA &
      (color == Color::WHITE ? promotionRank : ~Bitboard{});
  addMove(leftCaptureTargets & ~promotionRank, -1, Move::Type::CAPTURE);
  addPromotionMove(leftCaptureTargets & promotionRank, -1);

  const Bitboard rightCaptureTargets =
      (color == Color::WHITE ? (pawns << 9) : (pawns >> 7)) &
      (~empty | enPassant) & ~cantCapture & notFileH &
      (color == Color::WHITE ? promotionRank : ~Bitboard{});
  addMove(rightCaptureTargets & ~promotionRank, 1, Move::Type::CAPTURE);
  addPromotionMove(rightCaptureTargets & promotionRank, 1);

  const Bitboard pushPromoteTargets =
      (color == Color::WHITE ? (pawns << 8) : (pawns >> 8)) & empty &
      promotionRank;
  addPromotionMove(pushPromoteTargets, 0);
}

void Moves::possibleN(std::vector<chess_board::Move> &allMoves,
                      const chess_board::Color color,
                      const chess_board::Bitboard cantCapture,
                      chess_board::Bitboard knights) const {
  using namespace chess_board;

  while (knights) {
    int knightIndex = std::countr_zero(knights);
    const bool isOnTopHalf = knightIndex > 18;
    const bool isOnLeftHalf = knightIndex % 8 < 4;
    Bitboard possibilities =
        (isOnTopHalf ? masks_.KNIGHT_SPAN << (knightIndex - 18)
                     : masks_.KNIGHT_SPAN >> (18 - knightIndex)) &
        (isOnLeftHalf ? ~masks_.FILE_GH : ~masks_.FILE_AB) & ~cantCapture;

    while (possibilities) {
      int possibilitiesIndex = std::countr_zero(possibilities);
      allMoves.push_back({Cords(knightIndex / 8, knightIndex % 8),
                          Cords(possibilitiesIndex / 8, possibilitiesIndex % 8),
                          Move::Type::NORMAL});

      possibilities &= possibilities - 1;
    }

    knights &= knights - 1;
  }
}

void Moves::possibleSliderMoves(std::vector<chess_board::Move> &allMoves,
                                const chess_board::Color color,
                                const PieceName pieceTypeInWhite,
                                const chess_board::Bitboard cantCapture,
                                const chess_board::Bitboard empty,
                                chess_board::Bitboard pieces) const {
  using namespace chess_board;

  Bitboard smallestPiece;
  Bitboard possibilities;
  int pieceIndex;
  int possibilitiesIndex;

  while (pieces) {
    pieceIndex = std::countr_zero(pieces);
    smallestPiece = pieces & (~pieces + 1);

    if (pieceTypeInWhite == PieceName::WHITE_BISHOPS) {
      possibilities =
          hypQuint(empty, smallestPiece,
                   masks_.DIAGONAL_MASKS1[pieceIndex / 8 + pieceIndex % 8]) |
          hypQuint(empty, smallestPiece,
                   masks_.DIAGONAL_MASKS2[pieceIndex / 8 + 7 - pieceIndex % 8]);
    } else if (pieceTypeInWhite == PieceName::WHITE_ROOKS) {
      possibilities =
          hypQuint(empty, smallestPiece, masks_.FILE_MASKS[pieceIndex % 8]) |
          hypQuint(empty, smallestPiece, masks_.RANK_MASKS[pieceIndex / 8]);
    } else { // if the pieces are queens
      possibilities =
          hypQuint(empty, smallestPiece,
                   masks_.DIAGONAL_MASKS1[pieceIndex / 8 + pieceIndex % 8]) |
          hypQuint(
              empty, smallestPiece,
              masks_.DIAGONAL_MASKS2[pieceIndex / 8 + 7 - pieceIndex % 8]) |
          hypQuint(empty, smallestPiece, masks_.FILE_MASKS[pieceIndex % 8]) |
          hypQuint(empty, smallestPiece, masks_.RANK_MASKS[pieceIndex / 8]);
    }
    possibilities &= ~cantCapture;

    while (possibilities) {
      int possibilitiesIndex = std::countr_zero(possibilities);
      allMoves.push_back(
          {Cords(7 - pieceIndex / 8, pieceIndex % 8),
           Cords(7 - possibilitiesIndex / 8, possibilitiesIndex % 8),
           Move::Type::NORMAL});

      possibilities &= possibilities - 1;
    }
    pieces &= pieces - 1;
  }
}

constexpr chess_board::Bitboard
Moves::hypQuint(const chess_board::Bitboard empty,
                const chess_board::Bitboard square,
                const chess_board::Bitboard mask) {
  return ((((mask & ~empty) - square * 2) ^
           reverse(reverse(mask & ~empty) - (reverse(square) * 2)))) &
         mask;
}

constexpr chess_board::Bitboard Moves::reverse(chess_board::Bitboard b) {
  b = (b & 0x5555555555555555) << 1 | ((b >> 1) & 0x5555555555555555);
  b = (b & 0x3333333333333333) << 2 | ((b >> 2) & 0x3333333333333333);
  b = (b & 0x0f0f0f0f0f0f0f0f) << 4 | ((b >> 4) & 0x0f0f0f0f0f0f0f0f);
  b = (b & 0x00ff00ff00ff00ff) << 8 | ((b >> 8) & 0x00ff00ff00ff00ff);

  return (b << 48) | ((b & 0xffff0000) << 16) | ((b >> 16) & 0xffff0000) |
         (b >> 48);
}

void Moves::possibleK(std::vector<chess_board::Move> &allMoves,
                      const chess_board::Color color,
                      const chess_board::Bitboard cantCapture,
                      const chess_board::Bitboard empty,
                      const chess_board::Bitboard threats,
                      const chess_board::Bitboard king) const {
  using namespace chess_board;

  const int kingIndex = std::countr_zero(king);
  const bool kingOnRightSide = kingIndex > 9;
  Bitboard possibilities =
      (kingOnRightSide ? masks_.KING_SPAN << (kingIndex - 9)
                       : masks_.KING_SPAN >> (9 - kingIndex)) &
      (kingIndex % 8 < 4 ? ~masks_.FILE_GH : ~masks_.FILE_AB) & ~cantCapture &
      ~threats;

  while (possibilities) {
    int possibilitiesLocation = std::countr_zero(possibilities);
    allMoves.push_back(
        {Cords(7 - kingIndex / 8, kingIndex % 8),
         Cords(7 - possibilitiesLocation / 8, possibilitiesLocation % 8),
         Move::Type::NORMAL});

    possibilities &= possibilities - 1;
  }

  auto addMoves = [&](chess_board::Bitboard possibilities,
                      Move::Type moveType) {
    while (possibilities) {
      int possibilitiesLocation = std::countr_zero(possibilities);
      allMoves.push_back(
          {Cords(7 - kingIndex / 8, kingIndex % 8),
           Cords(7 - possibilitiesLocation / 8, possibilitiesLocation % 8),
           moveType});

      possibilities &= possibilities - 1;
    }
  };

  const bool isKingFrontRow = kingIndex / 8 == 7;
  if (isKingFrontRow) {
    if (castles_.whiteLongCastle)
      addMoves(king >> 2 & empty & empty >> 1 & ~(threats >> 1) & ~threats,
               Move::Type::WHITE_CASTLE_QUEENSIDE);

    if (castles_.whiteShortCastle)
      addMoves(king << 2 & empty & empty << 1 & ~(threats << 1) & ~threats,
               Move::Type::WHITE_CASTLE_KINGSIDE);
  } else {
    if (castles_.blackLongCastle)
      addMoves(king >> 2 & empty & empty >> 1 & ~(threats >> 1) & ~threats,
               Move::Type::BLACK_CASTLE_QUEENSIDE);

    if (castles_.blackShortCastle)
      addMoves(king << 2 & empty & empty << 1 & ~(threats << 1) & ~threats,
               Move::Type::BLACK_CASTLE_KINGSIDE);
  }
}

chess_board::Bitboard
Moves::getThreatSquares(chess_board::Color color,
                        const chess_board::Pieces &pieces) const {
  const bool isWhite = (color == chess_board::Color::WHITE);

  chess_board::Bitboard pawns = isWhite ? pieces.whitePawns : pieces.blackPawns;
  chess_board::Bitboard knights =
      isWhite ? pieces.whiteKnights : pieces.blackKnights;
  chess_board::Bitboard bishops =
      isWhite ? pieces.whiteBishops : pieces.blackBishops;
  chess_board::Bitboard rooks = isWhite ? pieces.whiteRooks : pieces.blackRooks;
  chess_board::Bitboard queens =
      isWhite ? pieces.whiteQueens : pieces.blackQueens;
  chess_board::Bitboard king = isWhite ? pieces.whiteKings : pieces.blackKings;

  const chess_board::Bitboard occupied =
      pieces.whitePawns | pieces.whiteKnights | pieces.whiteBishops |
      pieces.whiteRooks | pieces.whiteQueens | pieces.whiteKings |
      pieces.blackPawns | pieces.blackKnights | pieces.blackBishops |
      pieces.blackRooks | pieces.blackQueens | pieces.blackKings;
  const chess_board::Bitboard empty = ~occupied;

  int pieceLocation;
  chess_board::Bitboard smallestPiece;
  chess_board::Bitboard possibilities;

  // Add pawn attacks
  chess_board::Bitboard allThreats =
      isWhite ? (pawns >> 7 & ~masks_.FILE_A) | (pawns >> 9 & ~masks_.FILE_H)
              : (pawns << 7 & ~masks_.FILE_H) | (pawns << 9 & ~masks_.FILE_A);

  // Add knight attacks
  while (knights) {
    pieceLocation = std::countr_zero(knights);
    possibilities =
        (pieceLocation > 18 ? masks_.KNIGHT_SPAN << (pieceLocation - 18)
                            : masks_.KNIGHT_SPAN >> (18 - pieceLocation));
    possibilities &=
        (pieceLocation % 8 < 4) ? ~masks_.FILE_GH : ~masks_.FILE_AB;
    allThreats |= possibilities;
    knights &= knights - 1;
  }

  // Add bishop attacks
  while (bishops) {
    smallestPiece = bishops & (~bishops + 1);
    pieceLocation = std::countr_zero(smallestPiece);
    allThreats |=
        hypQuint(
            empty, smallestPiece,
            masks_.DIAGONAL_MASKS1[pieceLocation / 8 + pieceLocation % 8]) |
        hypQuint(
            empty, smallestPiece,
            masks_.DIAGONAL_MASKS2[pieceLocation / 8 + 7 - pieceLocation % 8]);
    bishops ^= smallestPiece;
  }

  while (rooks) {
    smallestPiece = rooks & (~rooks + 1);
    pieceLocation = std::countr_zero(smallestPiece);
    allThreats |=
        hypQuint(empty, smallestPiece, masks_.FILE_MASKS[pieceLocation % 8]) |
        hypQuint(empty, smallestPiece, masks_.RANK_MASKS[pieceLocation / 8]);
    rooks ^= smallestPiece;
  }

  while (queens) {
    smallestPiece = queens & (~queens + 1);
    pieceLocation = std::countr_zero(smallestPiece);
    allThreats |=
        hypQuint(
            empty, smallestPiece,
            masks_.DIAGONAL_MASKS1[pieceLocation / 8 + pieceLocation % 8]) |
        hypQuint(
            empty, smallestPiece,
            masks_.DIAGONAL_MASKS2[pieceLocation / 8 + 7 - pieceLocation % 8]) |
        hypQuint(empty, smallestPiece, masks_.FILE_MASKS[pieceLocation % 8]) |
        hypQuint(empty, smallestPiece, masks_.RANK_MASKS[pieceLocation / 8]);
    queens ^= smallestPiece;
  }

  // Add king attacks
  pieceLocation = std::countr_zero(king);
  possibilities = (pieceLocation > 9 ? masks_.KING_SPAN << (pieceLocation - 9)
                                     : masks_.KING_SPAN >> (9 - pieceLocation));
  possibilities &= (pieceLocation % 8 < 4) ? ~masks_.FILE_GH : ~masks_.FILE_AB;
  allThreats |= possibilities;

  return allThreats;
}

} // namespace chess_moves