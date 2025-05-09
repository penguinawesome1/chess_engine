module;

export module chess.move_generator;

import chess.core;
import chess.board_state;
import chess.move;
import <vector>;

export namespace chess::move_generator {

export auto getPossibleMoves(const chess::board::BoardState &boardState)
    -> std::vector<chess::board::Move>;
export auto getThreatSquares(const chess::board::Color color,
                             const chess::board::BoardState &boardState)
    -> chess::board::Bitboard;

} // namespace chess::move_generator