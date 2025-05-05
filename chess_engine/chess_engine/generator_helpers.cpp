// #include "generator_helpers.hpp"
//
// namespace chess_move_generator {
//
// namespace {
//
// struct HypQuintParams {
//   chess_board::Bitboard empty, square, mask;
// };
//
// constexpr chess_board::Bitboard reverse(chess_board::Bitboard b) {
//   b = (b & 0x5555555555555555) << 1 | ((b >> 1) & 0x5555555555555555);
//   b = (b & 0x3333333333333333) << 2 | ((b >> 2) & 0x3333333333333333);
//   b = (b & 0x0f0f0f0f0f0f0f0f) << 4 | ((b >> 4) & 0x0f0f0f0f0f0f0f0f);
//   b = (b & 0x00ff00ff00ff00ff) << 8 | ((b >> 8) & 0x00ff00ff00ff00ff);
//
//   return (b << 48) | ((b & 0xffff0000) << 16) | ((b >> 16) & 0xffff0000) |
//          (b >> 48);
// }
//
// constexpr chess_board::Bitboard hypQuint(const HypQuintParams &params) {
//   return ((((params.mask & ~params.empty) - params.square * 2) ^
//            reverse(reverse(params.mask & ~params.empty) -
//                    (reverse(params.square) * 2)))) &
//          params.mask;
// }
//
// } // namespace
//
// #pragma region pawns
// PawnGenerator::PawnGenerator(const Params &params)
//     : color(params.boardState.getTurnColor()), pawns(params.pawns),
//       empty(params.boardState.getEmpty()),
//       friendlyPieces(params.boardState.getPieces()),
//       promotionRank(params.promotionRank),
//       pawnStartRank(params.pawnStartRank),
//       enPassantTargetSquare(params.enPassantTargetSquare),
//       moveList(params.moveList) {}
//
// void PawnGenerator::addPushOnceTargets() const {
//   const chess_board::Shift myShift =
//       isWhite() ? chess_board::Shift::UP : chess_board::Shift::DOWN;
//   const chess_board::Bitboard targets = shift(pawns, myShift) & empty;
//
//   addNormalMove(targets & ~promotionRank, static_cast<int>(myShift));
//   addPromotionMove(targets & promotionRank, static_cast<int>(myShift));
// }
//
// void PawnGenerator::addPushTwiceTargets() const {
//   const chess_board::Shift myShift =
//       isWhite() ? chess_board::Shift::UP : chess_board::Shift::DOWN;
//   const chess_board::Bitboard targets =
//       shift(shift(pawns & pawnStartRank, myShift) & empty, myShift) & empty;
//
//   addNormalMove(targets, 2 * static_cast<int>(myShift));
// }
//
// void PawnGenerator::addLeftCaptureTargets() const {
//   const chess_board::Shift myShift =
//       isWhite() ? chess_board::Shift::LEFT_UP :
//       chess_board::Shift::LEFT_DOWN;
//   const chess_board::Bitboard targets =
//       shift(pawns, myShift) & ~friendlyPieces &
//       ~chess_board::constants::FILE_H;
//
//   const int shiftNum = static_cast<int>(myShift);
//   addNormalMove(targets & occupied & ~promotionRank, shiftNum);
//   addPromotionMove(targets & occupied & promotionRank, shiftNum);
//   addNormalMove(targets & enPassantTargetSquare, shiftNum,
//                 chess_board::Move::Type::EN_PASSANT);
// }
//
// void PawnGenerator::addRightCaptureTargets() const {
//   const chess_board::Shift myShift =
//       isWhite() ? chess_board::Shift::RIGHT_UP :
//       chess_board::Shift::RIGHT_DOWN;
//   const chess_board::Bitboard targets =
//       shift(pawns, myShift) & ~friendlyPieces &
//       ~chess_board::constants::FILE_A;
//
//   const int shiftNum = static_cast<int>(myShift);
//   addNormalMove(targets & occupied & ~promotionRank, shiftNum);
//   addPromotionMove(targets & occupied & promotionRank, shiftNum);
//   addNormalMove(targets & enPassantTargetSquare, shiftNum,
//                 chess_board::Move::Type::EN_PASSANT);
// }
//
// void PawnGenerator::addNormalMove(chess_board::Bitboard pieces, int offset,
//                                   chess_board::Move::Type type) const {
//   chess_board::Bitboard currentPieces = pieces & ~promotionRank;
//
//   while (auto pieceInfo = getNextPiece(currentPieces)) {
//     moveList.push_back({pieceInfo->index, offset, type});
//   }
// }
//
// void PawnGenerator::addPromotionMove(chess_board::Bitboard pieces,
//                                      int offset) const {
//   const chess_board::Move::Type basePromotionType =
//       isWhite() ? chess_board::Move::Type::WHITE_PAWN_PROMOTION_KNIGHT
//                 : chess_board::Move::Type::BLACK_PAWN_PROMOTION_KNIGHT;
//
//   chess_board::Bitboard currentPieces = pieces & promotionRank;
//
//   while (auto pieceInfo = getNextPiece(currentPieces)) {
//     for (std::uint8_t i = 0; i < 4; ++i) {
//       moveList.push_back({pieceInfo->index, offset,
//                           static_cast<chess_board::Move::Type>(
//                               static_cast<int>(basePromotionType) + i)});
//     }
//   }
// }
//
// bool PawnGenerator::isWhite() const {
//   return color == chess_board::Color::WHITE;
// }
// #pragma endregion
//
// #pragma region knights
// KnightGenerator::KnightGenerator(const Params &params)
//     : moveList(params.moveList), color(params.boardState.color),
//       knights(params.knights), empty(params.boardState.empty),
//       occupied(params.boardState.occupied),
//       friendlyPieces(params.boardState.friendlyPieces) {}
//
// void KnightGenerator::addKnightTargets() const {
//   chess_board::Bitboard currentKnights = knights, possibilities;
//   std::uint8_t knightIndex;
//   std::uint8_t possibilitiesIndex;
//   bool isOnLeftHalf;
//
//   while (auto pieceInfo = getNextPiece(currentKnights)) {
//     knightIndex = pieceInfo->index;
//     isOnLeftHalf = knightIndex % 8 < 4;
//     possibilities =
//         (knightIndex > 18
//              ? chess_board::constants::KNIGHT_SPAN << (knightIndex - 18)
//              : chess_board::constants::KNIGHT_SPAN >> (18 - knightIndex)) &
//         (isOnLeftHalf ? ~chess_board::constants::FILE_GH
//                       : ~chess_board::constants::FILE_AB) &
//         ~friendlyPieces;
//
//     while (auto pieceInfo = getNextPiece(possibilities)) {
//       possibilitiesIndex = pieceInfo->index;
//       moveList.push_back({knightIndex, possibilitiesIndex});
//     }
//   }
// }
// #pragma endregion
//
// #pragma region sliders
// SliderGenerator::SliderGenerator(const Params &params)
//     : moveList(params.moveList), color(params.boardState.color),
//       bishops(params.bishops), rooks(params.rooks), queens(params.queens),
//       empty(params.boardState.empty), occupied(params.boardState.occupied),
//       friendlyPieces(params.boardState.friendlyPieces) {}
//
// void SliderGenerator::addMoves(std::uint8_t startIndex,
//                                chess_board::Bitboard possibilities) const {
//   std::uint8_t possibilitiesIndex = 0;
//   possibilities &= ~friendlyPieces;
//
//   while (auto pieceInfo = getNextPiece(possibilities)) {
//     moveList.push_back({startIndex, pieceInfo->index});
//   }
// }
//
// void SliderGenerator::addDiagonalTargets(bool isQueen) const {
//   chess_board::Bitboard pieces = isQueen ? queens : bishops,
//                         smallestPiece = 0ULL, possibilities = 0ULL;
//   std::uint8_t pieceIndex = 0;
//
//   while (auto pieceInfo = getNextPiece(pieces)) {
//     pieceIndex = pieceInfo->index;
//     smallestPiece = pieceInfo->board;
//
//     possibilities |= hypQuint(
//         {.empty = empty,
//          .square = smallestPiece,
//          .mask = chess_board::constants::DIAGONAL_MASKS1[pieceIndex % 8 +
//                                                          pieceIndex / 8]});
//     possibilities |= hypQuint(
//         {.empty = empty,
//          .square = smallestPiece,
//          .mask = chess_board::constants::DIAGONAL_MASKS2[7 - pieceIndex % 8 +
//                                                          pieceIndex / 8]});
//   }
//
//   addMoves(pieceIndex, possibilities);
// }
//
// void SliderGenerator::addOrthogonalTargets(bool isQueen) const {
//   chess_board::Bitboard pieces = isQueen ? queens : rooks, smallestPiece =
//   0ULL,
//                         possibilities = 0ULL;
//   std::uint8_t pieceIndex = 0;
//
//   while (auto pieceInfo = getNextPiece(pieces)) {
//     pieceIndex = pieceInfo->index;
//     smallestPiece = pieceInfo->board;
//
//     possibilities |=
//         hypQuint({.empty = empty,
//                   .square = smallestPiece,
//                   .mask = chess_board::constants::FILE_MASKS[pieceIndex %
//                   8]});
//     possibilities |=
//         hypQuint({.empty = empty,
//                   .square = smallestPiece,
//                   .mask = chess_board::constants::RANK_MASKS[pieceIndex /
//                   8]});
//   }
//
//   addMoves(pieceIndex, possibilities);
// }
//
// #pragma endregion
//
// #pragma region kings
// KingGenerator::KingGenerator(const Params &params)
//     : moveList(params.moveList), color(params.boardState.color),
//       kings(params.kings), empty(params.boardState.empty),
//       occupied(params.boardState.occupied),
//       friendlyPieces(params.boardState.friendlyPieces),
//       threats(params.threats), castles(params.castles) {}
//
// void KingGenerator::addMoves(std::uint8_t startIndex,
//                              chess_board::Bitboard possibilities,
//                              chess_board::Move::Type type) const {
//   while (auto pieceInfo = getNextPiece(possibilities)) {
//     moveList.push_back({startIndex, pieceInfo->index, type});
//   }
// }
//
// void KingGenerator::addKingTargets() const {
//   chess_board::Bitboard pieces = kings, possibilities;
//   std::uint8_t kingIndex;
//
//   while (auto pieceInfo = getNextPiece(pieces)) {
//     kingIndex = pieceInfo->index;
//     possibilities =
//         (kingIndex > 9 ? chess_board::constants::KING_SPAN << (kingIndex - 9)
//                        : chess_board::constants::KING_SPAN >> (9 -
//                        kingIndex)) &
//         (kingIndex % 8 < 4 ? ~chess_board::constants::FILE_GH
//                            : ~chess_board::constants::FILE_AB) &
//         ~friendlyPieces & ~threats;
//
//     while (auto pieceInfo = getNextPiece(possibilities)) {
//       moveList.push_back({kingIndex, pieceInfo->index});
//     }
//   }
// }
//
// void KingGenerator::addCastleTargets() const {
//   chess_board::Bitboard pieces = kings, smallestPiece = 0ULL;
//   uint8_t pieceIndex = 0;
//
//   while (auto pieceInfo = getNextPiece(pieces)) {
//     pieceIndex = pieceInfo->index;
//     smallestPiece = pieceInfo->board;
//
//     if (castles.whiteLong)
//       addMoves(pieceIndex,
//                chess_board::constants::RANK_1 & empty & ~threats &
//                    shift(empty & ~threats, chess_board::Shift::LEFT) &
//                    shift(smallestPiece, chess_board::Shift::LEFT_2),
//                chess_board::Move::Type::WHITE_CASTLE_QUEENSIDE);
//
//     if (castles.whiteShort)
//       addMoves(pieceIndex,
//                chess_board::constants::RANK_1 & empty & ~threats &
//                    shift(empty & ~threats, chess_board::Shift::RIGHT) &
//                    shift(smallestPiece, chess_board::Shift::RIGHT_2),
//                chess_board::Move::Type::WHITE_CASTLE_KINGSIDE);
//
//     if (castles.blackShort)
//       addMoves(pieceIndex,
//                chess_board::constants::RANK_8 & empty & ~threats &
//                    shift(empty & ~threats, chess_board::Shift::LEFT) &
//                    shift(smallestPiece, chess_board::Shift::LEFT_2),
//                chess_board::Move::Type::BLACK_CASTLE_KINGSIDE);
//
//     if (castles.blackLong)
//       addMoves(pieceIndex,
//                chess_board::constants::RANK_8 & empty & ~threats &
//                    shift(empty & ~threats, chess_board::Shift::RIGHT) &
//                    shift(smallestPiece, chess_board::Shift::RIGHT_2),
//                chess_board::Move::Type::BLACK_CASTLE_QUEENSIDE);
//   }
// }
// #pragma endregion
//
// } // namespace chess_move_generator