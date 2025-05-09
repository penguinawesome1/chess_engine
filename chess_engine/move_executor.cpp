module;

#include <optional>
#include <stdexcept>

module chess.move_executor;

import chess.masks;
import chess.shifts;
import chess.board_state;
import chess.move;

namespace chess::move_executor {

namespace {

// Helper to update castling rights based on the move
void updateCastlingRights(chess::board::BoardState &boardState,
                          const chess::board::Bitboard startBoard,
                          const chess::board::Bitboard endBoard,
                          const chess::board::Name movedPieceName) {
  // --- King Moves ---
  if (movedPieceName == chess::board::Name::KING) {
    boardState.updateCastlingRights(boardState.getTurnColor(), true, true);
  }

  // --- Rook Moves ---
  if (movedPieceName == chess::board::Name::ROOK) {
    boardState.updateCastlingRights(
        boardState.getTurnColor(),
        startBoard & (chess::board::H1 | chess::board::H8),
        startBoard & (chess::board::A1 | chess::board::A8));
  }

  // --- Rook Captured ---
  boardState.updateCastlingRights(
      getOppositeColor(boardState.getTurnColor()),
      boardState.getTurnColor() == chess::board::Color::WHITE
          ? (endBoard & chess::board::H8)
          : (endBoard & chess::board::H1),
      boardState.getTurnColor() == chess::board::Color::WHITE
          ? (endBoard & chess::board::A8)
          : (endBoard & chess::board::A1));
}

// Helper for pawn promotion
void handlePromotion(chess::board::BoardState &boardState,
                     const chess::board::Move move,
                     const chess::board::Bitboard endBoard) {
  if (auto promotionName = move.tryGetPromotionName()) {
    boardState.tryRemovePiece(endBoard);
    boardState.addPiece(*promotionName, boardState.getTurnColor(), endBoard);
  }
}

// Helper for castling - moves the rook
void handleCastlingRookMove(chess::board::BoardState &boardState,
                            const chess::board::Move::Type type) {
  switch (type) {
  case chess::board::Move::Type::WHITE_CASTLE_KINGSIDE:
    boardState.tryMovePiece(chess::board::H1, chess::board::F1);
    break;
  case chess::board::Move::Type::WHITE_CASTLE_QUEENSIDE:
    boardState.tryMovePiece(chess::board::A1, chess::board::D1);
    break;
  case chess::board::Move::Type::BLACK_CASTLE_KINGSIDE:
    boardState.tryMovePiece(chess::board::H8, chess::board::F8);
    break;
  case chess::board::Move::Type::BLACK_CASTLE_QUEENSIDE:
    boardState.tryMovePiece(chess::board::A8, chess::board::D8);
    break;
  default:
    // Not a castling move
    break;
  }
}

// Helper to handle en passant capture and update EP square
void handleEnPassant(chess::board::BoardState &boardState,
                     const chess::board::Bitboard startBoard,
                     const chess::board::Bitboard endBoard,
                     const chess::board::Move::Type moveType) {
  chess::board::Bitboard capturedPawnSquare = 0;

  // 1. Handle capture IF the current move IS en passant
  if (moveType == chess::board::Move::Type::EN_PASSANT) {
    if (boardState.getPieces(chess::board::Color::WHITE,
                             chess::board::Name::PAWN) &
        startBoard) { // White pawn moved
      capturedPawnSquare = chess::board::shiftSouth(endBoard);
      boardState.tryRemovePiece(capturedPawnSquare);
    } else { // Black pawn moved
      capturedPawnSquare = chess::board::shiftNorth(endBoard);
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

    if (name == chess::board::Name::PAWN) {
      if (color == chess::board::Color::WHITE) {
        if (chess::board::shiftNorth(chess::board::shiftNorth(startBoard)) ==
            endBoard) {
          // White pawn double push, EP square is one step behind
          boardState.setEnPassantSquare(chess::board::shiftSouth(endBoard));
        }
      } else {
        if (chess::board::shiftSouth(chess::board::shiftSouth(startBoard)) ==
            endBoard) {
          // Black pawn double push, EP square is one step behind
          boardState.setEnPassantSquare(chess::board::shiftNorth(endBoard));
        }
      }
    }
  }
}

} // namespace

// Main function to execute a move
void doMove(chess::board::BoardState &boardState,
            const chess::board::Move &move) {
  const chess::board::Bitboard startBoard = move.getStartBoard(),
                               endBoard = move.getEndBoard();
  const chess::board::Move::Type moveType = move.getType();

  // 1. Identify the moving piece BEFORE modifying boards
  std::optional<chess::board::Name> movedPieceNameOpt = std::nullopt;
  if (auto result = boardState.getPieceInfo(startBoard)) {
    movedPieceNameOpt = result->name;
  } else {
    throw std::logic_error(
        "Error: No board found that matches start board position.");
  }
  const chess::board::Name movedPieceName = *movedPieceNameOpt;

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

} // namespace chess::move_executor