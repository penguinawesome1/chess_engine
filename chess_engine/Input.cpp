#include "Input.hpp"

namespace chess_input {

chess_board::GameParams gatherInputs() {
  using namespace chess_board;

  GameType gameType;
  OpponentType opponentType;
  Color playerColor;
  int depth;

  std::cout << "Enter game type (0 for STANDARD, 1 for CHESS960): ";
  int gameTypeInput;
  std::cin >> gameTypeInput;
  gameType = static_cast<GameType>(gameTypeInput);

  std::cout << "Play against (0 for HUMAN, 1 for ENGINE): ";
  int opponentTypeInput;
  std::cin >> opponentTypeInput;
  opponentType = static_cast<OpponentType>(opponentTypeInput);

  if (opponentType == OpponentType::HUMAN)
    return {gameType, opponentType, Color::WHITE, 0};

  std::cout << "Play as (0 for WHITE, 1 for BLACK): ";
  int playerColorInput;
  std::cin >> playerColorInput;
  playerColor = static_cast<Color>(playerColorInput);

  std::cout << "Enter search depth for engine: ";
  std::cin >> depth;

  return {gameType, opponentType, playerColor, depth};
}

chess_board::Move getPlayerMove(const chess_board::Color color,
                                const chess_moves::Moves &moves,
                                const chess_board::Pieces &pieces) {
  using namespace chess_board;

  std::vector<Move> possibleMoves = moves.getPossibleMoves(color, pieces);

  while (true) {
    std::string moveInput;
    std::cout << "Enter your move (xyxy): ";
    std::cin >> moveInput;

    std::size_t x1 = static_cast<std::size_t>(moveInput[0] - '0');
    std::size_t y1 = static_cast<std::size_t>(moveInput[1] - '0');
    std::size_t x2 = static_cast<std::size_t>(moveInput[2] - '0');
    std::size_t y2 = static_cast<std::size_t>(moveInput[3] - '0');

    Move move(Cords(x1, y1), Cords(x2, y2));

    std::vector<Move> chosenMoves;
    for (const auto &possibleMove : possibleMoves) {
      if (move.startSquare == possibleMove.startSquare &&
          move.endSquare == possibleMove.endSquare) {
        chosenMoves.push_back(move);
      }
    }

    const std::size_t chosenMovesSize = chosenMoves.size();

    if (chosenMovesSize == 0)
      continue;

    if (chosenMovesSize > 1) {
      std::size_t counter = 0;

      for (const Move &moveOption : chosenMoves) {
        std::cout << "Number: " << counter << " Move: ";
        move.print();
        counter++;
      }
      std::cout << "\n";

      int choice;
      std::cout << "Enter the number of your choice: ";
      std::cin >> choice;
      move = chosenMoves[choice];
    }

    return move;
  }
}

} // namespace chess_input