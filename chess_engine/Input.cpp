#include "Input.hpp"
#include <cctype>
#include <iostream>
#include <limits>
#include <tuple>

namespace chess_input {

std::tuple<chess_board::GameType, chess_board::OpponentType, chess_board::Color,
           int>
gatherInputs() {
  using namespace chess_board;

  const GameType gameType = getGameType();
  const OpponentType opponentType = getOpponent();
  const Color playerColor =
      opponentType == OpponentType::ENGINE ? getPlayerColor() : Color::WHITE;
  const int depth = opponentType == OpponentType::ENGINE ? getEngineDepth() : 0;

  std::cout << "\n";

  return std::make_tuple(gameType, opponentType, playerColor, depth);
}

chess_board::GameType getGameType() {
  char gameType;
  while (true) {
    std::cout << "Play [c]hess or c[h]ess960? ";
    if (std::cin >> gameType) {
      gameType = toupper(gameType);
      if (gameType == 'C' || gameType == 'H') {
        break;
      }
    }
    std::cout << "Invalid input. Please enter 'c' or 'h'.\n";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  return gameType == 'C' ? chess_board::GameType::CHESS
                         : chess_board::GameType::CHESS960;
}

chess_board::OpponentType getOpponent() {
  char opponent;
  while (true) {
    std::cout << "Verse [p]layer or [e]ngine? ";
    if (std::cin >> opponent) {
      opponent = toupper(opponent);
      if (opponent == 'P' || opponent == 'E') {
        break;
      }
    }
    std::cout << "Invalid input. Please enter 'p' or 'e'.\n";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  return opponent == 'P' ? chess_board::OpponentType::PLAYER
                         : chess_board::OpponentType::ENGINE;
}

chess_board::Color getPlayerColor() {
  char playerColor;
  while (true) {
    std::cout << "Play as [w]hite or [b]lack? ";
    if (std::cin >> playerColor) {
      playerColor = toupper(playerColor);
      if (playerColor == 'W' || playerColor == 'B') {
        break;
      }
    }
    std::cout << "Invalid input. Please enter 'w' or 'b'.\n";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  return playerColor == 'W' ? chess_board::Color::WHITE
                            : chess_board::Color::BLACK;
}

int getEngineDepth() {
  constexpr int minDepth = 1;
  constexpr int maxDepth = 5;

  int depth;
  while (true) {
    std::cout << "What engine depth (int " << minDepth << "-" << maxDepth
              << ")? ";
    if (std::cin >> depth && depth >= minDepth && depth <= maxDepth) {
      break;
    } else {
      std::cout << "Invalid input. Please enter an integer between " << minDepth
                << " and " << maxDepth << ".\n";
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
  }
  return depth;
}

} // namespace chess_input