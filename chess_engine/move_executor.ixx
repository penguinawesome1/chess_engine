module;

export module chess.move_executor;

import chess.core;
import chess.board_state;
import chess.move;

export namespace chess::move_executor {

export void doMove(chess::board::BoardState &boardState,
                   const chess::board::Move &move);

} // namespace chess::move_executor