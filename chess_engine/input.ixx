module;

export module chess.input;

import chess.move;
import chess.core;
import <vector>;

export namespace chess::input {

export enum class OpponentType { HUMAN, ENGINE };

export struct GameParams {
  OpponentType opponentType;
  chess::board::Color playerColor;
  int depth;
};

export auto gatherInputs() -> GameParams;
export auto getPlayerMove(const std::vector<chess::board::Move> &possibleMoves)
    -> chess::board::Move;

} // namespace chess::input