#include "Board.hpp"

namespace chess_board {

Board::Board(const chess_moves::Moves &moves)
    : pieces_(), moves_(moves), bestMove_{}, moveC_(2), isWhiteTurn_(true) {}

void Board::handleTurn(const chess_board::GameParams &inputs) {
  const std::vector<Move> possibleMoves =
      moves_.getPossibleMoves(inputs.playerColor);
  Move move;

  for (const auto &move : possibleMoves) {
    move.print();
  }

  if (inputs.opponentType == OpponentType::ENGINE) {
    const bool isPlayerTurn =
        (isWhiteTurn_ && inputs.playerColor == Color::WHITE) ||
        (!isWhiteTurn_ && inputs.playerColor == Color::BLACK);

    MinimaxParams initialParams{};
    initialParams.depth = inputs.depth;
    initialParams.alpha = -std::numeric_limits<float>::infinity();
    initialParams.beta = std::numeric_limits<float>::infinity();
    initialParams.isRootCall = true;

    if (!isPlayerTurn)
      minimax(initialParams);

    move = isPlayerTurn
               ? chess_input::getPlayerMove(inputs.playerColor, moves_, pieces_)
               : bestMove_;
  } else { // OpponentType::HUMAN
    move = chess_input::getPlayerMove(inputs.playerColor, moves_, pieces_);
  }

  moves_.doMove(move);

  printBoard();

  isWhiteTurn_ = !isWhiteTurn_;
  moveC_++;
}

void Board::startGame() {
  const chess_board::GameParams inputs = chess_input::gatherInputs();
  std::cout << "\n";

  if (inputs.gameType == GameType::CHESS960)
    initializeChess960(); // if playing chess960 shuffle the files
  arrayToBitboard();
  printBoard();

  while (!isGameOver()) {
    handleTurn(inputs);
  }

  if (isCheckmate(Color::BLACK)) {
    std::cout << "White wins!";
  } else if (isCheckmate(Color::WHITE)) {
    std::cout << "Black wins!";
  } else {
    std::cout << "It's a stalemate!";
  }
}

void Board::initializeChess960() {
  char pieces[8] = {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'};

  std::default_random_engine rng(static_cast<unsigned int>(std::time(nullptr)));
  std::shuffle(std::begin(pieces), std::end(pieces), rng);

  for (int i = 0; i < 8; ++i) {
    chessBoard_[7][i] = pieces[i];
    chessBoard_[0][i] = tolower(pieces[i]);
  }
}

void Board::arrayToBitboard() {
  static const std::map<char, Bitboard(Pieces::*)> pieceMap = {
      {'P', &Pieces::whitePawns},   {'N', &Pieces::whiteKnights},
      {'B', &Pieces::whiteBishops}, {'R', &Pieces::whiteRooks},
      {'Q', &Pieces::whiteQueens},  {'K', &Pieces::whiteKings},
      {'p', &Pieces::blackPawns},   {'n', &Pieces::blackKnights},
      {'b', &Pieces::blackBishops}, {'r', &Pieces::blackRooks},
      {'q', &Pieces::blackQueens},  {'k', &Pieces::blackKings}};

  for (std::size_t rank = 0; rank < 8; ++rank) {
    for (std::size_t file = 0; file < 8; ++file) {
      char pieceChar = chessBoard_[rank][file];
      auto it = pieceMap.find(pieceChar);
      if (it != pieceMap.end()) {
        std::size_t index = rank * 8 + file;
        const Bitboard mask = 1ULL << index;
        (pieces_.*(it->second)) |= mask;
      }
    }
  }
}

void Board::printBoard() const {
  std::string piece;

  for (std::size_t rank = 0; rank < 8; ++rank) {
    std::cout << std::left << std::setw(3) << (7 - rank);

    for (std::size_t file = 0; file < 8; ++file) {
      const std::size_t index = rank * 8 + file;
      const Bitboard mask = 1ULL << index;

      if (pieces_.whitePawns & mask) {
        piece = 'P';
      } else if (pieces_.whiteKnights & mask) {
        piece = 'N';
      } else if (pieces_.whiteBishops & mask) {
        piece = 'B';
      } else if (pieces_.whiteRooks & mask) {
        piece = 'R';
      } else if (pieces_.whiteQueens & mask) {
        piece = 'Q';
      } else if (pieces_.whiteKings & mask) {
        piece = 'K';
      } else if (pieces_.blackPawns & mask) {
        piece = 'p';
      } else if (pieces_.blackKnights & mask) {
        piece = 'n';
      } else if (pieces_.blackBishops & mask) {
        piece = 'b';
      } else if (pieces_.blackRooks & mask) {
        piece = 'r';
      } else if (pieces_.blackQueens & mask) {
        piece = 'q';
      } else if (pieces_.blackKings & mask) {
        piece = 'k';
      } else {
        piece = '.';
      }

      std::cout << std::setw(2) << piece;

      if (file == 7) {
        std::cout << '\n';
      }
    }
  }

  std::cout << "\n   0 1 2 3 4 5 6 7\n" << std::endl;
}

float Board::minimax(const MinimaxParams &params) {
  if (params.depth == 0 || isGameOver())
    return evaluate(params.depth);

  if (isWhiteTurn_) {
    const std::vector<Move> moves = moves_.getPossibleMoves(Color::WHITE);
    float maxScore = std::numeric_limits<float>::lowest();

    for (const Move &move : moves) {
      moves_.doMove(move);

      if (isChecked(Color::WHITE)) {
        moves_.undoMove();
        continue;
      }

      MinimaxParams nextParams = params;
      nextParams.depth--;
      nextParams.isRootCall = false;
      const float score = minimax(nextParams);
      moves_.undoMove();

      if (params.isRootCall && score > maxScore)
        bestMove_ = move;
      maxScore = std::max(score, maxScore);
      nextParams.alpha = std::max(params.alpha, maxScore);
      if (nextParams.beta <= nextParams.alpha)
        return maxScore;
    }
    return maxScore;
  } else {
    const std::vector<Move> moves = moves_.getPossibleMoves(Color::BLACK);
    float minScore = std::numeric_limits<float>::max();

    for (const Move &move : moves) {
      moves_.doMove(move);

      if (isChecked(Color::BLACK)) {
        moves_.undoMove();
        continue;
      }

      MinimaxParams nextParams = params;
      nextParams.depth--;
      nextParams.isRootCall = false;
      const float score = minimax(nextParams);
      moves_.undoMove();

      if (params.isRootCall && score < minScore)
        bestMove_ = move;
      minScore = std::min(score, minScore);
      nextParams.beta = std::min(params.beta, minScore);
      if (nextParams.beta <= nextParams.alpha)
        return minScore;
    }
    return minScore;
  }
}

float Board::evaluate(const int depth) const {
  static constexpr float CHECKMATE_SCORE = 1000.0f;

  if (isCheckmate(Color::WHITE))
    return CHECKMATE_SCORE + depth; // add depth to prioritize faster mates

  if (isCheckmate(Color::BLACK))
    return -CHECKMATE_SCORE - depth;

  if (isStalemate(Color::WHITE) || isStalemate(Color::BLACK))
    return 0.0f;

  return materialScore() + positionScore();
}

bool Board::isChecked(Color color) const {
  const Bitboard king =
      color == Color::WHITE ? pieces_.whiteKings : pieces_.blackKings;
  return king & moves_.getThreatSquares(color);
}

bool Board::isCheckmate(Color color) const {
  return isChecked(color) && !hasLegalMoves(color);
}

bool Board::isStalemate(Color color) const {
  return (!isChecked(color) && !hasLegalMoves(color)) || onlyKingsLeft();
}

bool Board::isGameOver() const {
  return isCheckmate(Color::WHITE) || isCheckmate(Color::BLACK) ||
         isStalemate(Color::WHITE) || isStalemate(Color::BLACK) ||
         onlyKingsLeft();
}

bool Board::hasLegalMoves(Color color) const {
  const std::vector<Move> moves = moves_.getPossibleMoves(color);
  for (const Move &move : moves) {
    Board tempBoard = *this;
    tempBoard.moves_.doMove(move);
    if (!tempBoard.isChecked(color))
      return true;
  }
  return false;
}

bool Board::onlyKingsLeft() const {
  return std::popcount(pieces_.whitePawns | pieces_.whiteKnights |
                       pieces_.whiteBishops | pieces_.whiteRooks |
                       pieces_.whiteQueens | pieces_.blackPawns |
                       pieces_.blackKnights | pieces_.blackBishops |
                       pieces_.blackRooks | pieces_.blackQueens) == 0;
}

float Board::materialScore() const {
  return 1.0f * std::popcount(pieces_.whitePawns) +
         3.0f * std::popcount(pieces_.whiteKnights | pieces_.whiteBishops) +
         5.0f * std::popcount(pieces_.whiteRooks) +
         9.0f * std::popcount(pieces_.whiteQueens) -
         1.0f * std::popcount(pieces_.blackPawns) -
         3.0f * std::popcount(pieces_.blackKnights | pieces_.blackBishops) -
         5.0f * std::popcount(pieces_.blackRooks) -
         9.0f * std::popcount(pieces_.blackQueens);
}

float Board::positionScore() const {
  auto processPieces = [&](Bitboard pieces, const Color color,
                           const bool isKing) -> float {
    const int multiplier = color == Color::WHITE ? 1 : -1;
    float localScore = 0.0f;

    while (pieces) {
      int square = std::countr_zero(pieces);
      pieces &= pieces - 1;

      int row = square / 8;
      int col = square % 8;

      localScore += (isKing ? kingLocationValues[row][col]
                            : pieceLocationValues[row][col]) *
                    multiplier;
    }
    return localScore;
  };

  const Bitboard whiteNonKingPieces =
      pieces_.whitePawns | pieces_.whiteKnights | pieces_.whiteBishops |
      pieces_.whiteRooks | pieces_.whiteQueens;
  const Bitboard blackNonKingPieces =
      pieces_.blackPawns | pieces_.blackKnights | pieces_.blackBishops |
      pieces_.blackRooks | pieces_.blackQueens;

  constexpr bool IS_KING = true;

  return processPieces(whiteNonKingPieces, Color::WHITE, !IS_KING) +
         processPieces(blackNonKingPieces, Color::BLACK, !IS_KING) +
         processPieces(pieces_.whiteKings, Color::WHITE, IS_KING) +
         processPieces(pieces_.blackKings, Color::BLACK, IS_KING);
}

} // namespace chess_board