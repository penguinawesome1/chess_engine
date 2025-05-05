#include "board.hpp"

namespace chess_board {

namespace {

void initPieces(BoardState &boardState) {
  static const char boardLayout[8][8] = {
      {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
      {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
      {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
      {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
      {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
      {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
      {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
      {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}};

  auto getPieceNameFromChar = [](char pieceChar) -> std::optional<Name> {
    switch (tolower(pieceChar)) {
    case 'p':
      return Name::PAWN;
    case 'n':
      return Name::KNIGHT;
    case 'b':
      return Name::BISHOP;
    case 'r':
      return Name::ROOK;
    case 'q':
      return Name::QUEEN;
    case 'k':
      return Name::KING;
    default:
      return std::nullopt; // Indicate an empty space or invalid char
    }
  };

  for (std::uint8_t rank = 0; rank < 8; ++rank) {
    for (std::uint8_t file = 0; file < 8; ++file) {
      char pieceChar = boardLayout[rank][file];
      if (auto pieceNameOpt = getPieceNameFromChar(pieceChar)) {
        const Bitboard mask = 1ULL << (rank * 8 + file);
        Color color = isupper(pieceChar) ? Color::WHITE : Color::BLACK;
        boardState.addPiece(*pieceNameOpt, color, mask);
      }
    }
  }
}

} // namespace

std::vector<Move> Board::getAndPrintPossibleMoves() const {
  const std::vector<Move> possibleMoves =
      chess_move_generator::getPossibleMoves(boardState_);
  for (const auto &move : possibleMoves) {
    std::cout << move.getMoveString() << "\n";
  }
  std::cout << std::endl;
  return possibleMoves;
}

Move Board::getEngineMove(const int depth) {
  return minimax({.depth = depth,
                  .alpha = -std::numeric_limits<float>::infinity(),
                  .beta = std::numeric_limits<float>::infinity(),
                  .isRootCall = true})
      .bestMove;
}

void Board::handleTurn(const chess_input::GameParams &inputs) {
  const bool isPlayerMove =
      inputs.opponentType == chess_input::OpponentType::HUMAN ||
      (inputs.opponentType == chess_input::OpponentType::ENGINE &&
       (boardState_.getTurnColor() == inputs.playerColor));

  Move move = isPlayerMove
                  ? chess_input::getPlayerMove(getAndPrintPossibleMoves())
                  : getEngineMove(inputs.depth);

  chess_move_executor::doMove(boardState_, move);
  printBoard(boardState_);
  boardState_.swapTurnColor();
}

void Board::startGame() {
  const chess_input::GameParams inputs = chess_input::gatherInputs();

  initPieces(boardState_);
  printBoard(boardState_);

  while (true || !isGameOver(boardState_.getTurnColor())) {
    handleTurn(inputs);
  }

  std::cout << isCheckmate(Color::BLACK) ? "White wins!"
  : isCheckmate(Color::WHITE)            ? "Black wins!"
                                         : "It's a tie!";
}

void Board::printBoard(BoardState &boardState) {
  auto getPieceAtMask = [&](Bitboard mask) -> char {
    if (boardState.getPieces(Color::WHITE, Name::PAWN) & mask)
      return 'P';
    if (boardState.getPieces(Color::WHITE, Name::KNIGHT) & mask)
      return 'N';
    if (boardState.getPieces(Color::WHITE, Name::BISHOP) & mask)
      return 'B';
    if (boardState.getPieces(Color::WHITE, Name::ROOK) & mask)
      return 'R';
    if (boardState.getPieces(Color::WHITE, Name::QUEEN) & mask)
      return 'Q';
    if (boardState.getPieces(Color::WHITE, Name::KING) & mask)
      return 'K';
    if (boardState.getPieces(Color::BLACK, Name::PAWN) & mask)
      return 'p';
    if (boardState.getPieces(Color::BLACK, Name::KNIGHT) & mask)
      return 'n';
    if (boardState.getPieces(Color::BLACK, Name::BISHOP) & mask)
      return 'b';
    if (boardState.getPieces(Color::BLACK, Name::ROOK) & mask)
      return 'r';
    if (boardState.getPieces(Color::BLACK, Name::QUEEN) & mask)
      return 'q';
    if (boardState.getPieces(Color::BLACK, Name::KING) & mask)
      return 'k';
    return '.';
  };

  for (std::uint8_t rank = 0; rank < 8; ++rank) {
    std::cout << std::left << std::setw(3) << (7 - rank);

    for (std::uint8_t file = 0; file < 8; ++file) {
      const chess_board::Bitboard mask = 1ULL << (rank * 8 + file);
      std::cout << std::setw(2) << getPieceAtMask(mask);

      if (file == 7)
        std::cout << '\n';
    }
  }

  std::cout << "\n   0 1 2 3 4 5 6 7\n" << std::endl;
}

Board::MinimaxResult Board::minimax(const MinimaxParams &params) {
  const auto &[depth, alpha, beta, isRootCall] = params;

  const Color turnColor = boardState_.getTurnColor();

  if (depth == 0 || isGameOver(turnColor))
    return {evaluate(turnColor, depth), {}};

  Move bestMove{};
  float bestScore = turnColor == Color::WHITE
                        ? std::numeric_limits<float>::lowest()
                        : std::numeric_limits<float>::max();

  for (const Move &move : chess_move_generator::getPossibleMoves(boardState_)) {
    BoardState pieceCopy = boardState_;
    boardState_.swapTurnColor();

    chess_move_executor::doMove(boardState_, move);
    if (isChecked(turnColor)) {
      boardState_ = pieceCopy;
      continue;
    }

    float score = minimax({depth - 1, alpha, beta, false}).score;

    boardState_ = pieceCopy;
    boardState_.swapTurnColor();

    if (turnColor == Color::WHITE ? score > bestScore : score < bestScore) {
      bestScore = score;
      if (isRootCall)
        bestMove = move;
    }

    const bool branchToPrune =
        (turnColor == Color::WHITE && bestScore >= beta) ||
        (turnColor == Color::BLACK && bestScore <= alpha);
    if (branchToPrune)
      break;
  }
  return {bestScore, bestMove};
}

float Board::evaluate(const Color color, const std::size_t depth) {
  static constexpr float CHECKMATE_SCORE = 1000.0f;

  if (isCheckmate(Color::WHITE))
    return CHECKMATE_SCORE + depth; // add depth to prioritize faster mates

  if (isCheckmate(Color::BLACK))
    return -CHECKMATE_SCORE - depth;

  if (isStalemate(Color::WHITE) || isStalemate(Color::BLACK))
    return 0.0f;

  return boardState_.materialScore() + boardState_.positionScore();
}

bool Board::isChecked(const Color color) const {
  const Bitboard king = color == Color::WHITE
                            ? boardState_.getPieces(Color::WHITE, Name::KING)
                            : boardState_.getPieces(Color::BLACK, Name::KING);
  return king & chess_move_generator::getThreatSquares(color, boardState_);
}

bool Board::isCheckmate(const Color color) {
  return isChecked(color) && !hasLegalMoves(color);
}

bool Board::isStalemate(const Color color) {
  return (!isChecked(color) && !hasLegalMoves(color)) ||
         boardState_.onlyKingsLeft();
}

bool Board::isGameOver(const Color color) {
  return isCheckmate(Color::WHITE) || isCheckmate(Color::BLACK) ||
         isStalemate(Color::WHITE) || isStalemate(Color::BLACK) ||
         boardState_.onlyKingsLeft();
}

bool Board::hasLegalMoves(const Color color) {
  for (const Move &move : chess_move_generator::getPossibleMoves(boardState_)) {
    BoardState pieceCopy = boardState_;
    chess_move_executor::doMove(boardState_, move);
    if (!isChecked(color)) {
      boardState_ = pieceCopy;
      return true;
    }
    boardState_ = pieceCopy;
  }
  return false;
}

} // namespace chess_board