module;

#include <iostream>
#include <vector>

module chess.input;

namespace chess::input {

namespace {

auto resolveAmbiguousMove(const std::vector<chess::board::Move> &chosenMoves)
    -> chess::board::Move {
  std::uint8_t counter = 0;

  for (const chess::board::Move &move : chosenMoves) {
    std::cout << "#" << counter++ << " - Move: " << move.getMoveString();
  }
  std::cout << "\n";

  int choice;
  std::cout << "Enter the number of your choice: ";
  std::cin >> choice;
  return chosenMoves[choice];
}

auto getMatchingMoves(const chess::board::Move move,
                      const std::vector<chess::board::Move> &possibleMoves)
    -> std::vector<chess::board::Move> {
  std::vector<chess::board::Move> matchingMoves;
  for (const auto &possibleMove : possibleMoves) {
    if (move.getStartSquare() == possibleMove.getStartSquare() &&
        move.getEndSquare() == possibleMove.getEndSquare()) {
      matchingMoves.push_back(possibleMove);
    }
  }
  return matchingMoves;
}

auto getMoveFromString(const std::string &moveString) -> chess::board::Move {
  return chess::board::Move(
      chess::board::Cords(moveString[0] - 'a', 7 - (moveString[1] - '1')),
      chess::board::Cords(moveString[2] - 'a', 7 - (moveString[3] - '1')));
}

} // namespace

auto getPlayerMove(const std::vector<chess::board::Move> &possibleMoves)
    -> chess::board::Move {
  while (true) {
    std::string moveString;
    std::cout << "Enter your move (xyxy): ";
    std::cin >> moveString;
    std::cout << "\n";

    chess::board::Move move = getMoveFromString(moveString);

    std::vector<chess::board::Move> matchingMoves =
        getMatchingMoves(move, possibleMoves);

    if (matchingMoves.size() == 1)
      move = matchingMoves[0]; // in case the only move that matches is special

    if (matchingMoves.size() > 1)
      move = resolveAmbiguousMove(matchingMoves);

    if (matchingMoves.size() != 0)
      return move;
  }
}

GameParams gatherInputs() {
  OpponentType opponentType;
  chess::board::Color playerColor;
  int depth;

  std::cout << "Play against (0 for HUMAN, 1 for ENGINE): ";
  int opponentTypeInput;
  std::cin >> opponentTypeInput;
  opponentType = static_cast<OpponentType>(opponentTypeInput);

  if (opponentType == OpponentType::HUMAN) {
    std::cout << "\n";
    return {opponentType, chess::board::Color::WHITE, 0};
  }

  std::cout << "Play as (0 for WHITE, 1 for BLACK): ";
  int playerColorInput;
  std::cin >> playerColorInput;
  playerColor = static_cast<chess::board::Color>(playerColorInput);

  std::cout << "Enter search depth for engine: ";
  std::cin >> depth;

  std::cout << "\n";

  return {opponentType, playerColor, depth};
}

} // namespace chess::input