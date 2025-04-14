#include "Board.hpp"

namespace chess_board {

Board::Board(const chess_moves::Moves &moves)
    : pieces_(), moves_(moves), bestMove_{}, moveC_(2), isWhiteTurn_(true),
      pieceLocationValues{{-.5f, -.4f, -.4f, -.4f, -.4f, -.4f, -.4f, -.5f},
                          {-.4f, -.2f, .0f, .0f, .0f, .0f, -.2f, -.4f},
                          {-.4f, .0f, .1f, .2f, .2f, .1f, .0f, -.4f},
                          {-.4f, .0f, .2f, .25f, .25f, .2f, .0f, -.4f},
                          {-.4f, .0f, .2f, .25f, .25f, .2f, .0f, -.4f},
                          {-.4f, .0f, .1f, .2f, .2f, .1f, .0f, -.4f},
                          {-.4f, -.2f, .0f, .0f, .0f, .0f, -.2f, -.4f},
                          {-.5f, -.4f, -.4f, -.4f, -.4f, -.4f, -.4f, -.5f}},
      kingLocationValues{{-.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f},
                         {-.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f},
                         {-.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f},
                         {-.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f},
                         {-.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f},
                         {-.2f, -.2f, -.2f, -.2f, -.2f, -.2f, -.2f, -.2f},
                         {.2f, .2f, .0f, .0f, .0f, .0f, .2f, .2f},
                         {.2f, .3f, .1f, .0f, .0f, .1f, .3f, .2f}} {
  // initialize chessboard.
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      chessBoard_[i][j] = {{'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
                           {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
                           {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                           {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                           {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                           {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                           {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
                           {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}};
    }
  }
}

void Board::startGame() {
  const auto [gameType, opponentType, playerColor, depth] =
      chess_input::gatherInputs();

  // if playing chess960, shuffle rank1 and rank8 the same way
  if (gameType == GameType::CHESS960)
    initializeChess960();
  arrayToBitboard();
  printBoard();

  float score = 0;

  // if (opponentType == OpponentType::ENGINE) {
  //   while (!gameOver()) {
  //     const bool isPlayerTurn = (isWhiteTurn && playerColor == Color::WHITE)
  //     ||
  //                               (!isWhiteTurn && playerColor ==
  //                               Color::BLACK);
  //     if (!isPlayerTurn)
  //       score = minimax();

  //    const std::string possibleMoves;
  //    const std::string move = PLAYER_TURN ? getPlayerMove() : bestMove;
  //    moves.doMove();

  //    printBoard();

  //    isWhiteTurn = !isWhiteTurn;
  //    moveC++;
  //  }
  //} else { // player opponenet
  //  while (!evaluate1.gameOver()) {
  //    const std::string move = getPlayerMove();
  //    moves.doMove();
  //    printBoard();

  //    whiteTurn = !whiteTurn;
  //    moveC++;
  //  }
  //}

  // const bool blackChecked = pieces.blackKings & moves.otherThreats();
  // const bool whiteChecked = pieces.whiteKings & moves.otherThreats();

  // if (!isWhiteTurn && blackChecked) {
  //   std::cout << "White wins!";
  // } else if (isWhiteTurn && whiteChecked) {
  //   std::cout << "Black wins!";
  // } else {
  //   std::cout << "It's a stalemate!";
  // }
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
  for (std::size_t rank = 0; rank < 8; ++rank) {
    for (std::size_t file = 0; file < 8; ++file) {
      std::size_t index = rank * 8 + file;
      const Bitboard mask = 1ULL << index;

      switch (chessBoard_[rank][file]) {
      case 'P':
        pieces_.whitePawns |= mask;
        break;
      case 'N':
        pieces_.whiteKnights |= mask;
        break;
      case 'B':
        pieces_.whiteBishops |= mask;
        break;
      case 'R':
        pieces_.whiteRooks |= mask;
        break;
      case 'Q':
        pieces_.whiteQueens |= mask;
        break;
      case 'K':
        pieces_.whiteKings |= mask;
        break;
      case 'p':
        pieces_.blackPawns |= mask;
        break;
      case 'n':
        pieces_.blackKnights |= mask;
        break;
      case 'b':
        pieces_.blackBishops |= mask;
        break;
      case 'r':
        pieces_.blackRooks |= mask;
        break;
      case 'q':
        pieces_.blackQueens |= mask;
        break;
      case 'k':
        pieces_.blackKings |= mask;
        break;
      }
    }
  }
}

void Board::printBoard() const {
  std::string piece;

  for (std::size_t rank = 0; rank < 8; ++rank) {
    std::cout << std::left << std::setw(3) << (7 - rank);

    for (std::size_t file = 0; file < 8; ++file) {
      std::size_t index = rank * 8 + file;
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

float Board::minimax(const int depth, float alpha, float beta,
                     const bool isRootCall) {

  if (depth == 0 || isGameOver())
    return evaluate(depth);

  if (isWhiteTurn_) {
    const std::vector<MoveCords> moves = moves_.possibleMovesWhite();
    float maxScore = std::numeric_limits<float>::lowest();
    for (const MoveCords &move : moves) {
      moves_.doMove(move);

      const bool whiteChecked =
          pieces_.whiteKings & moves_.areOtherThreats(false);
      if (whiteChecked) {
        moves_.undoMove();
        continue;
      }

      const float score = minimax(depth - 1, alpha, beta, false);
      moves_.undoMove();

      if (isRootCall && score > maxScore) {
        bestMove_ = move;
      }
      maxScore = std::max(score, maxScore);
      alpha = std::max(alpha, maxScore);
      if (beta <= alpha) {
        return maxScore;
      }
    }
    return maxScore;
  } else {
    const std::vector<MoveCords> moves = moves_.possibleMovesBlack();
    float minScore = std::numeric_limits<float>::max();
    for (const MoveCords &move : moves) {
      moves_.doMove(move);

      const bool BLACK_CHECKED =
          pieces_.blackKings & moves_.areOtherThreats(true);
      if (BLACK_CHECKED) {
        moves_.undoMove();
        continue;
      }

      const float score = minimax(depth - 1, alpha, beta, false);
      moves_.undoMove();

      if (isRootCall && score < minScore) {
        bestMove_ = move;
      }
      minScore = std::min(score, minScore);
      beta = std::min(beta, minScore);
      if (beta <= alpha) {
        return minScore;
      }
    }
    return minScore;
  }
}

float Board::evaluate(const int depth) {

  const bool BLACK_CHECKED =
      !isWhiteTurn_ && pieces.blackKing & moves_.otherThreats(true);
  const bool BLACK_MATED = BLACK_CHECKED && !areBlackMoves();
  if (BLACK_MATED)
    return 1000 + depth; // add depth to prioritize faster mates

  const bool whiteChecked =
      isWhiteTurn_ && pieces.whiteKing & moves_.otherThreats(false);
  const bool whiteMated = whiteChecked && !areWhiteMoves();
  if (whiteMated)
    return -1000 - depth; // subtract depth to prioritize faster mates

  const bool stalemate = (isWhiteTurn_ && !areWhiteMoves()) ||
                         (!isWhiteTurn_ && !areBlackMoves()) ||
                         notEnoughPieces();
  if (stalemate)
    return 0;

  return materialScore() + positionScore();
}

bool Board::isGameOver() {
  return (isWhiteTurn_ && !areWhiteMoves()) ||
         (!isWhiteTurn_ && !areBlackMoves()) || notEnoughPieces();
}

bool Board::areWhiteMoves() {
  const std::string MOVES = moves_.possibleMovesWhite();
  for (int i = 0; i < MOVES.length(); i += 5) {
    moves_.doMove(MOVES.substr(i, 5));
    const bool WHITE_CHECKED = pieces_.whiteKings & moves_.otherThreats(false);
    if (!WHITE_CHECKED) {
      return true;
    }
    moves_.undoMove();
  }
  return false;
}

bool Board::areBlackMoves() {
  const std::string MOVES = moves_.possibleMovesBlack();
  for (int i = 0; i < MOVES.length(); i += 5) {
    moves_.doMove(MOVES.substr(i, 5));
    const bool BLACK_CHECKED = pieces_.blackKings & moves_.otherThreats(true);
    if (!BLACK_CHECKED) {
      return true;
    }
    moves_.undoMove();
  }
  return false;
}

constexpr bool Board::notEnoughPieces() const {
  return std::popcount(pieces_.whitePawns | pieces_.whiteKnights |
                       pieces_.whiteBishops | pieces_.whiteRooks |
                       pieces_.whiteQueens | pieces_.blackPawns |
                       pieces_.blackKnights | pieces_.blackBishops |
                       pieces_.blackRooks | pieces_.blackQueens) == 0;
}

constexpr int Board::materialScore() const {
  return std::popcount(pieces_.whitePawns) +
         3 * std::popcount(pieces_.whiteKnights | pieces_.whiteBishops) +
         5 * std::popcount(pieces_.whiteRooks) +
         9 * std::popcount(pieces_.whiteQueens) -
         std::popcount(pieces_.blackPawns) -
         3 * std::popcount(pieces_.blackKnights | pieces_.blackBishops) -
         5 * std::popcount(pieces_.blackRooks) -
         9 * std::popcount(pieces_.blackQueens);
}

constexpr float Board::positionScore() const {
  float score = 0;

  // location value for white pieces (excluding king)
  Bitboard pieces = pieces_.whitePawns | pieces_.whiteKnights |
                    pieces_.whiteBishops | pieces_.whiteRooks |
                    pieces_.whiteQueens;
  Bitboard smallestPiece;
  int pieceLocation;
  while (pieces) {
    smallestPiece = pieces & -pieces;
    pieces ^= smallestPiece;
    pieceLocation = std::countl_zero(smallestPiece);
    score += pieceLocationValues[pieceLocation / 8][pieceLocation % 8];
  }

  // location value for black pieces (excluding king)
  pieces = pieces_.blackPawns | pieces_.blackKnights | pieces_.blackBishops |
           pieces_.blackRooks | pieces_.blackQueens;
  while (pieces) {
    smallestPiece = pieces & -pieces;
    pieces ^= smallestPiece;
    pieceLocation = std::countl_zero(smallestPiece);
    score -= pieceLocationValues[pieceLocation / 8][pieceLocation % 8];
  }

  // location value for white king
  if (pieces_.whiteKings) {
    pieceLocation = std::countl_zero(pieces_.whiteKings);
    score += kingLocationValues[pieceLocation / 8][pieceLocation % 8];
  }

  // location value for black king
  if (pieces_.blackKings) {
    pieceLocation = std::countl_zero(pieces_.blackKings);
    score -= kingLocationValues[7 - pieceLocation / 8][pieceLocation % 8];
  }

  return score;
}

};

} // namespace chess_board