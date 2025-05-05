#include "move_executor.hpp"

namespace chess_move_executor {

namespace {

// Helper to update castling rights based on the move
void updateCastlingRights(chess_board::BoardState &boardState,
                          const chess_board::Bitboard startBoard,
                          const chess_board::Bitboard endBoard,
                          const chess_board::Name movedPieceName) {
  // --- King Moves ---
  if (movedPieceName == chess_board::Name::KING) {
    boardState.updateCastlingRights(boardState.getTurnColor(), true, true);
  }

  // --- Rook Moves ---
  if (movedPieceName == chess_board::Name::ROOK) {
    boardState.updateCastlingRights(
        boardState.getTurnColor(),
        startBoard & (chess_board::constants::H1 | chess_board::constants::H8),
        startBoard & (chess_board::constants::A1 | chess_board::constants::A8));
  }

  // --- Rook Captured ---
  boardState.updateCastlingRights(
      getOppositeColor(boardState.getTurnColor()),
      boardState.getTurnColor() == chess_board::Color::WHITE
          ? (endBoard & chess_board::constants::H8)
          : (endBoard & chess_board::constants::H1),
      boardState.getTurnColor() == chess_board::Color::WHITE
          ? (endBoard & chess_board::constants::A8)
          : (endBoard & chess_board::constants::A1));
}

// Helper for pawn promotion
void handlePromotion(chess_board::BoardState &boardState,
                     const chess_board::Move move,
                     const chess_board::Bitboard endBoard) {
  if (auto promotionName = move.tryGetPromotionName()) {
    boardState.tryRemovePiece(endBoard);
    boardState.addPiece(*promotionName, boardState.getTurnColor(), endBoard);
  }
}

// Helper for castling - moves the rook
void handleCastlingRookMove(chess_board::BoardState &boardState,
                            const chess_board::Move::Type type) {
  // Use switch for clarity
  switch (type) {
  case chess_board::Move::Type::WHITE_CASTLE_KINGSIDE:
    boardState.tryMovePiece(chess_board::constants::H1,
                            chess_board::constants::F1);
    break;
  case chess_board::Move::Type::WHITE_CASTLE_QUEENSIDE:
    boardState.tryMovePiece(chess_board::constants::A1,
                            chess_board::constants::D1);
    break;
  case chess_board::Move::Type::BLACK_CASTLE_KINGSIDE:
    boardState.tryMovePiece(chess_board::constants::H8,
                            chess_board::constants::F8);
    break;
  case chess_board::Move::Type::BLACK_CASTLE_QUEENSIDE:
    boardState.tryMovePiece(chess_board::constants::A8,
                            chess_board::constants::D8);
    break;
  default:
    // Not a castling move
    break;
  }
}

// Helper to handle en passant capture and update EP square
void handleEnPassant(chess_board::BoardState &boardState,
                     const chess_board::Bitboard startBoard,
                     const chess_board::Bitboard endBoard,
                     const chess_board::Move::Type moveType) {
  chess_board::Bitboard capturedPawnSquare = 0;

  // 1. Handle capture IF the current move IS en passant
  if (moveType == chess_board::Move::Type::EN_PASSANT) {
    if (boardState.getPieces(chess_board::Color::WHITE,
                             chess_board::Name::PAWN) &
        startBoard) { // White pawn moved
      capturedPawnSquare = chess_board::shiftSouth(endBoard);
      boardState.tryRemovePiece(capturedPawnSquare);
    } else { // Black pawn moved
      capturedPawnSquare = chess_board::shiftNorth(endBoard);
      boardState.tryRemovePiece(capturedPawnSquare);
    }
    // Note: The actual capture happens here, clearing the opponent pawn
    // board. handleCapture() shouldn't run for the EP target square itself.
  }

  // 2. Update the potential en passant square for the *next* turn
  // Reset en passant square by default
  boardState.setEnPassantSquare(std::nullopt);

  // Check if the current move was a double pawn push
  if (auto result = boardState.getPieceInfo(startBoard); result.has_value()) {
    auto &[boardRef, name, color] = *result;

    if (name == chess_board::Name::PAWN) {
      if (color == chess_board::Color::WHITE) {
        if (chess_board::shiftNorth(chess_board::shiftNorth(startBoard)) ==
            endBoard) {
          // White pawn double push, EP square is one step behind
          boardState.setEnPassantSquare(chess_board::shiftSouth(endBoard));
        }
      } else {
        if (chess_board::shiftSouth(chess_board::shiftSouth(startBoard)) ==
            endBoard) {
          // Black pawn double push, EP square is one step behind
          boardState.setEnPassantSquare(chess_board::shiftNorth(endBoard));
        }
      }
    }
  }
}

} // namespace

// Main function to execute a move
void doMove(chess_board::BoardState &boardState,
            const chess_board::Move &move) {
  const chess_board::Bitboard startBoard = move.getStartBoard(),
                              endBoard = move.getEndBoard();
  const chess_board::Move::Type moveType = move.getType();

  // 1. Identify the moving piece BEFORE modifying boards
  std::optional<chess_board::Name> movedPieceNameOpt = std::nullopt;
  if (auto result = boardState.getPieceInfo(startBoard)) {
    movedPieceNameOpt = result->name;
  } else {
    throw std::logic_error(
        "Error: No board found that matches start board position.");
  }
  const chess_board::Name movedPieceName = *movedPieceNameOpt;

  // 2. Handle En Passant Capture (removes the captured pawn) and updates the en
  // passant square for the *next* turn.
  handleEnPassant(boardState, startBoard, endBoard, moveType);

  // 3. Handle Regular Capture (if not en passant)
  boardState.tryRemovePiece(endBoard);

  // 4. Move the actual piece
  boardState.tryMovePiece(startBoard, endBoard);

  // 5. Handle Promotion
  handlePromotion(boardState, move, endBoard);

  // 6. Handle Castling Rook Movement
  handleCastlingRookMove(boardState, moveType);

  // 7. Update Castling Rights
  updateCastlingRights(boardState, startBoard, endBoard, movedPieceName);

  boardState.swapTurnColor();
}

} // namespace chess_move_executor