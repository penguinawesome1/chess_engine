module;

import chess.cords;
import chess.core;
import <array>;
import <optional>;
import <stdexcept>;
import <string>;

export module chess.move;

export namespace chess::board {

export struct Move {
  enum class Type : uint8_t {
    NORMAL,
    EN_PASSANT,
    WHITE_CASTLE_KINGSIDE,
    WHITE_CASTLE_QUEENSIDE,
    BLACK_CASTLE_KINGSIDE,
    BLACK_CASTLE_QUEENSIDE,
    WHITE_PAWN_PROMOTION_KNIGHT,
    WHITE_PAWN_PROMOTION_BISHOP,
    WHITE_PAWN_PROMOTION_ROOK,
    WHITE_PAWN_PROMOTION_QUEEN,
    BLACK_PAWN_PROMOTION_KNIGHT,
    BLACK_PAWN_PROMOTION_BISHOP,
    BLACK_PAWN_PROMOTION_ROOK,
    BLACK_PAWN_PROMOTION_QUEEN,
    // Max value should fit in 4 bits (0-15)
    // Ensure this enum doesn't exceed 16 values if using 4 bits
  };

  // Bits 0-5: Start square index (0-63)
  // Bits 6-11: End square index (0-63)
  // Bits 12-15: Move Type/Flags (0-15)
  uint16_t data_;

  constexpr Move() : data_(0) {}
  constexpr Move(Cords start, Cords end, Move::Type type = Type::NORMAL)
      : data_(static_cast<uint16_t>(start.getIndex()) |
              (static_cast<uint16_t>(end.getIndex()) << 6) |
              (static_cast<uint16_t>(type) << 12)) {}

  [[nodiscard]] constexpr Cords getStartSquare() const {
    return Cords(static_cast<uint8_t>(data_ & 0x3F)); // Mask bits 0-5
  }
  [[nodiscard]] constexpr Cords getEndSquare() const {
    return Cords(static_cast<uint8_t>((data_ >> 6) &
                                      0x3F)); // Shift right 6, mask bits 6-11
  }
  [[nodiscard]] constexpr Bitboard getStartBoard() const {
    return 1ULL << (data_ & 0x3F); // Mask bits 0-5 to get index
  }
  [[nodiscard]] constexpr Bitboard getEndBoard() const {
    return 1ULL << ((data_ >> 6) &
                    0x3F); // Shift right 6, mask bits 6-11 to get index
  }

  [[nodiscard]] constexpr Move::Type getType() const {
    // Shift right 12, mask bits 12-15
    return static_cast<Move::Type>((data_ >> 12) & 0x0F);
  }

  [[nodiscard]] constexpr bool isPromotion() const {
    auto type = getType();
    return type >= Type::WHITE_PAWN_PROMOTION_KNIGHT &&
           type <= Type::BLACK_PAWN_PROMOTION_QUEEN;
  }

  [[nodiscard]] constexpr std::optional<Name> tryGetPromotionName() const {
    if (!isPromotion()) {
      return std::nullopt;
    }

    switch (getType()) {
    case Type::WHITE_PAWN_PROMOTION_KNIGHT:
    case Type::BLACK_PAWN_PROMOTION_KNIGHT:
      return Name::KNIGHT;
    case Type::WHITE_PAWN_PROMOTION_BISHOP:
    case Type::BLACK_PAWN_PROMOTION_BISHOP:
      return Name::BISHOP;
    case Type::WHITE_PAWN_PROMOTION_ROOK:
    case Type::BLACK_PAWN_PROMOTION_ROOK:
      return Name::ROOK;
    case Type::WHITE_PAWN_PROMOTION_QUEEN:
    case Type::BLACK_PAWN_PROMOTION_QUEEN:
      return Name::QUEEN;
    default:
      throw std::logic_error(
          "Function isPromotion is not flagging moves correctly.");
      return std::nullopt;
    }
  }

  [[nodiscard]] std::string getMoveString() const {
    static const std::array<std::string, 14> moveTypeStrings = {
        "NORMAL",
        "EN_PASSANT",
        "WHITE_CASTLE_KINGSIDE",
        "WHITE_CASTLE_QUEENSIDE",
        "BLACK_CASTLE_KINGSIDE",
        "BLACK_CASTLE_QUEENSIDE",
        "WHITE_PAWN_PROMOTION_KNIGHT",
        "WHITE_PAWN_PROMOTION_BISHOP",
        "WHITE_PAWN_PROMOTION_ROOK",
        "WHITE_PAWN_PROMOTION_QUEEN",
        "BLACK_PAWN_PROMOTION_KNIGHT",
        "BLACK_PAWN_PROMOTION_BISHOP",
        "BLACK_PAWN_PROMOTION_ROOK",
        "BLACK_PAWN_PROMOTION_QUEEN"};

    return getStartSquare().getAlgebraic() + getEndSquare().getAlgebraic() +
           " Type: " + moveTypeStrings[static_cast<int>(getType())];
  }

  constexpr bool operator==(const Move &other) const {
    return data_ == other.data_;
  }
  constexpr bool operator!=(const Move &other) const {
    return !(*this == other);
  }
};

} // namespace chess::board