#include "input.hpp"

namespace chess_input {

namespace {

chess_board::Move
resolveAmbiguousMove(const std::vector<chess_board::Move> &chosenMoves) {
  std::uint8_t counter = 0;

  for (const chess_board::Move &move : chosenMoves) {
    std::cout << "#" << counter++ << " - Move: " << move.getMoveString();
  }
  std::cout << "\n";

  int choice;
  std::cout << "Enter the number of your choice: ";
  std::cin >> choice;
  return chosenMoves[choice];
}

std::vector<chess_board::Move>
getMatchingMoves(const chess_board::Move move,
                 const std::vector<chess_board::Move> &possibleMoves) {
  std::vector<chess_board::Move> matchingMoves;
  for (const auto &possibleMove : possibleMoves) {
    if (move.getStartSquare() == possibleMove.getStartSquare() &&
        move.getEndSquare() == possibleMove.getEndSquare()) {
      matchingMoves.push_back(possibleMove);
    }
  }
  return matchingMoves;
}

chess_board::Move getMoveFromString(const std::string &moveString) {
  std::uint8_t x1 = static_cast<std::uint8_t>(moveString[0] - '0'),
               y1 = static_cast<std::uint8_t>(moveString[1] - '0'),
               x2 = static_cast<std::uint8_t>(moveString[2] - '0'),
               y2 = static_cast<std::uint8_t>(moveString[3] - '0');

  return chess_board::Move(chess_board::Cords(x1, y1),
                           chess_board::Cords(x2, y2));
}

} // namespace

chess_board::Move
getPlayerMove(const std::vector<chess_board::Move> &possibleMoves) {
  while (true) {
    std::string moveString;
    std::cout << "Enter your move (xyxy): ";
    std::cin >> moveString;
    std::cout << "\n";

    chess_board::Move move = getMoveFromString(moveString);

    std::vector<chess_board::Move> matchingMoves =
        getMatchingMoves(move, possibleMoves);

    if (matchingMoves.size() == 1) // remove if not testing
      move = matchingMoves[0]; // in case the only move that matches is special

    if (matchingMoves.size() > 1)
      move = resolveAmbiguousMove(matchingMoves);

    // if (matchingMoves.size() != 0)
    return move;
  }
}

GameParams gatherInputs() {
  OpponentType opponentType;
  chess_board::Color playerColor;
  int depth;

  std::cout << "Play against (0 for HUMAN, 1 for ENGINE): ";
  int opponentTypeInput;
  std::cin >> opponentTypeInput;
  opponentType = static_cast<OpponentType>(opponentTypeInput);

  if (opponentType == OpponentType::HUMAN) {
    std::cout << "\n";
    return {opponentType, chess_board::Color::WHITE, 0};
  }

  std::cout << "Play as (0 for WHITE, 1 for BLACK): ";
  int playerColorInput;
  std::cin >> playerColorInput;
  playerColor = static_cast<chess_board::Color>(playerColorInput);

  std::cout << "Enter search depth for engine: ";
  std::cin >> depth;

  std::cout << "\n";

  return {opponentType, playerColor, depth};
}

} // namespace chess_input