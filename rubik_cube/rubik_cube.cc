
#include"rubik_cube.h"

#include<memory.h> // for memcpy().

#include <immintrin.h> // AVX2


const __m256i pure_front_back_left_right = _mm256_set_epi8(
	RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red
	, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange
	, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green
	, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue
);
const __m128i pure_up_and_down = _mm_set_epi8(
	RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White
	, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow
);

constexpr char solved_cube[48] = {
	RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White,
	RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow,
	RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red,
	RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange,
	RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green,
	RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue
};

RubikCube::RubikCube(const char *ptr)
    : overall_{ptr[0],  ptr[1],  ptr[2],  ptr[3],  ptr[4],  ptr[5],  ptr[6], ptr[7],  ptr[8],  ptr[9],  ptr[10], ptr[11], ptr[12], ptr[13], ptr[14], ptr[15], ptr[16], ptr[17], ptr[18], ptr[19], ptr[20], ptr[21], ptr[22], ptr[23], ptr[24], ptr[25], ptr[26], ptr[27], ptr[28], ptr[29], ptr[30], ptr[31], ptr[32], ptr[33], ptr[34], ptr[35], ptr[36], ptr[37], ptr[38], ptr[39], ptr[40], ptr[41], ptr[42], ptr[43], ptr[44], ptr[45], ptr[46], ptr[47]},
      up_(overall_ + 8 * 0),
      down_(overall_ + 8 * 1),
      front_(overall_ + 8 * 2),
      back_(overall_ + 8 * 3),
      left_(overall_ + 8 * 4),
      right_(overall_ + 8 * 5) {}

RubikCube::RubikCube(Colors up_0, Colors up_1, Colors up_2, Colors up_3,
                     Colors up_4, Colors up_5, Colors up_6, Colors up_7,
                     Colors down_0, Colors down_1, Colors down_2, Colors down_3,
                     Colors down_4, Colors down_5, Colors down_6, Colors down_7,
                     Colors front_0, Colors front_1, Colors front_2,
                     Colors front_3, Colors front_4, Colors front_5,
                     Colors front_6, Colors front_7, Colors back_0,
                     Colors back_1, Colors back_2, Colors back_3, Colors back_4,
                     Colors back_5, Colors back_6, Colors back_7, Colors left_0,
                     Colors left_1, Colors left_2, Colors left_3, Colors left_4,
                     Colors left_5, Colors left_6, Colors left_7,
                     Colors right_0, Colors right_1, Colors right_2,
                     Colors right_3, Colors right_4, Colors right_5,
                     Colors right_6, Colors right_7)
    : overall_{up_0,    up_1,    up_2,    up_3,    up_4,    up_5,    up_6,
               up_7,    down_0,  down_1,  down_2,  down_3,  down_4,  down_5,
               down_6,  down_7,  front_0, front_1, front_2, front_3, front_4,
               front_5, front_6, front_7, back_0,  back_1,  back_2,  back_3,
               back_4,  back_5,  back_6,  back_7,  left_0,  left_1,  left_2,
               left_3,  left_4,  left_5,  left_6,  left_7,  right_0, right_1,
               right_2, right_3, right_4, right_5, right_6, right_7},
      up_(overall_ + 8 * 0),
      down_(overall_ + 8 * 1),
      front_(overall_ + 8 * 2),
      back_(overall_ + 8 * 3),
      left_(overall_ + 8 * 4),
      right_(overall_ + 8 * 5) {}

bool RubikCube::isSolved() const {
#ifdef USE_AVX2
  return (
      int(0xffffffff) ==
          _mm256_movemask_epi8(_mm256_cmpeq_epi8(
              _mm256_set_epi8(
                  front_[0], front_[1], front_[2], front_[3], front_[4],
                  front_[5], front_[6], front_[7], back_[0], back_[1], back_[2],
                  back_[3], back_[4], back_[5], back_[6], back_[7], left_[0],
                  left_[1], left_[2], left_[3], left_[4], left_[5], left_[6],
                  left_[7], right_[0], right_[1], right_[2], right_[3],
                  right_[4], right_[5], right_[6], right_[7]),
              pure_front_back_left_right)) &&
      0xffff ==
          _mm_movemask_epi8(_mm_cmpeq_epi8(
              _mm_set_epi8(up_[0], up_[1], up_[2], up_[3], up_[4], up_[5],
                           up_[6], up_[7], down_[0], down_[1], down_[2],
                           down_[3], down_[4], down_[5], down_[6], down_[7]),
              pure_up_and_down)));
#else
  return 0 == memcmp(solved_cube, overall_, sizeof(overall_));
#endif  // USE_AVX2
}

void RubikCube::transform(CubeTransforms t) {
  char temp[3] = {0};
  switch (t) {
    case RubikCube::L:
      temp[0] = up_[0];
      temp[1] = up_[3];
      temp[2] = up_[5];
      up_[0] = back_[0];
      up_[3] = back_[3];
      up_[5] = back_[5];
      back_[0] = down_[0];
      back_[3] = down_[3];
      back_[5] = down_[5];
      down_[0] = front_[0];
      down_[3] = front_[3];
      down_[5] = front_[5];
      front_[0] = temp[0];
      front_[3] = temp[1];
      front_[5] = temp[2];
      temp[0] = left_[0];
      temp[1] = left_[1];
      left_[0] = left_[5];
      left_[5] = left_[7];
      left_[7] = left_[2];
      left_[2] = temp[0];
      left_[1] = left_[3];
      left_[3] = left_[6];
      left_[6] = left_[4];
      left_[4] = temp[1];
      break;
    case RubikCube::R:
      temp[0] = up_[2];
      temp[1] = up_[4];
      temp[2] = up_[7];
      up_[2] = front_[2];
      up_[4] = front_[4];
      up_[7] = front_[7];
      front_[2] = down_[2];
      front_[4] = down_[4];
      front_[7] = down_[7];
      down_[2] = back_[2];
      down_[4] = back_[4];
      down_[7] = back_[7];
      back_[2] = temp[0];
      back_[4] = temp[1];
      back_[7] = temp[2];
      temp[0] = right_[0];
      temp[1] = right_[1];
      right_[0] = right_[5];
      right_[5] = right_[7];
      right_[7] = right_[2];
      right_[2] = temp[0];
      right_[1] = right_[3];
      right_[3] = right_[6];
      right_[6] = right_[4];
      right_[4] = temp[1];
      break;
    case RubikCube::U:
      temp[0] = back_[5];
      temp[1] = back_[6];
      temp[2] = back_[7];
      back_[5] = left_[2];
      back_[6] = left_[1];
      back_[7] = left_[0];
      left_[0] = front_[0];
      left_[1] = front_[1];
      left_[2] = front_[2];
      front_[0] = right_[0];
      front_[1] = right_[1];
      front_[2] = right_[2];
      right_[0] = temp[2];
      right_[1] = temp[1];
      right_[2] = temp[0];
      temp[0] = up_[0];
      temp[1] = up_[1];
      up_[0] = up_[5];
      up_[5] = up_[7];
      up_[7] = up_[2];
      up_[2] = temp[0];
      up_[1] = up_[3];
      up_[3] = up_[6];
      up_[6] = up_[4];
      up_[4] = temp[1];
      break;
    case RubikCube::D:
      temp[0] = back_[0];
      temp[1] = back_[1];
      temp[2] = back_[2];
      back_[0] = right_[7];
      back_[1] = right_[6];
      back_[2] = right_[5];
      right_[5] = front_[5];
      right_[6] = front_[6];
      right_[7] = front_[7];
      front_[5] = left_[5];
      front_[6] = left_[6];
      front_[7] = left_[7];
      left_[5] = temp[2];
      left_[6] = temp[1];
      left_[7] = temp[0];
      temp[0] = down_[0];
      temp[1] = down_[1];
      down_[0] = down_[5];
      down_[5] = down_[7];
      down_[7] = down_[2];
      down_[2] = temp[0];
      down_[1] = down_[3];
      down_[3] = down_[6];
      down_[6] = down_[4];
      down_[4] = temp[1];
      break;
    case RubikCube::F:
      temp[0] = down_[0];
      temp[1] = down_[1];
      temp[2] = down_[2];
      down_[0] = right_[5];
      down_[1] = right_[3];
      down_[2] = right_[0];
      right_[5] = up_[7];
      right_[3] = up_[6];
      right_[0] = up_[5];
      up_[7] = left_[2];
      up_[6] = left_[4];
      up_[5] = left_[7];
      left_[2] = temp[0];
      left_[4] = temp[1];
      left_[7] = temp[2];
      temp[0] = front_[0];
      temp[1] = front_[1];
      front_[0] = front_[5];
      front_[5] = front_[7];
      front_[7] = front_[2];
      front_[2] = temp[0];
      front_[1] = front_[3];
      front_[3] = front_[6];
      front_[6] = front_[4];
      front_[4] = temp[1];
      break;
    case RubikCube::B:
      temp[0] = up_[0];
      temp[1] = up_[1];
      temp[2] = up_[2];
      up_[0] = right_[2];
      up_[1] = right_[4];
      up_[2] = right_[7];
      right_[2] = down_[7];
      right_[4] = down_[6];
      right_[7] = down_[5];
      down_[7] = left_[5];
      down_[6] = left_[3];
      down_[5] = left_[0];
      left_[5] = temp[0];
      left_[3] = temp[1];
      left_[0] = temp[2];
      temp[0] = back_[0];
      temp[1] = back_[1];
      back_[0] = back_[5];
      back_[5] = back_[7];
      back_[7] = back_[2];
      back_[2] = temp[0];
      back_[1] = back_[3];
      back_[3] = back_[6];
      back_[6] = back_[4];
      back_[4] = temp[1];
      break;
    case RubikCube::L_prime:
      temp[0] = up_[0];
      temp[1] = up_[3];
      temp[2] = up_[5];
      up_[0] = front_[0];
      up_[3] = front_[3];
      up_[5] = front_[5];
      front_[0] = down_[0];
      front_[3] = down_[3];
      front_[5] = down_[5];
      down_[0] = back_[0];
      down_[3] = back_[3];
      down_[5] = back_[5];
      back_[0] = temp[0];
      back_[3] = temp[1];
      back_[5] = temp[2];
      temp[0] = left_[0];
      temp[1] = left_[1];
      left_[0] = left_[2];
      left_[2] = left_[7];
      left_[7] = left_[5];
      left_[5] = temp[0];
      left_[1] = left_[4];
      left_[4] = left_[6];
      left_[6] = left_[3];
      left_[3] = temp[1];
      break;
    case RubikCube::R_prime:
      temp[0] = up_[2];
      temp[1] = up_[4];
      temp[2] = up_[7];
      up_[2] = back_[2];
      up_[4] = back_[4];
      up_[7] = back_[7];
      back_[2] = down_[2];
      back_[4] = down_[4];
      back_[7] = down_[7];
      down_[2] = front_[2];
      down_[4] = front_[4];
      down_[7] = front_[7];
      front_[2] = temp[0];
      front_[4] = temp[1];
      front_[7] = temp[2];
      temp[0] = right_[0];
      temp[1] = right_[1];
      right_[0] = right_[2];
      right_[2] = right_[7];
      right_[7] = right_[5];
      right_[5] = temp[0];
      right_[1] = right_[4];
      right_[4] = right_[6];
      right_[6] = right_[3];
      right_[3] = temp[1];
      break;
    case RubikCube::U_prime:
      temp[0] = back_[5];
      temp[1] = back_[6];
      temp[2] = back_[7];
      back_[5] = right_[2];
      back_[6] = right_[1];
      back_[7] = right_[0];
      right_[0] = front_[0];
      right_[1] = front_[1];
      right_[2] = front_[2];
      front_[0] = left_[0];
      front_[1] = left_[1];
      front_[2] = left_[2];
      left_[0] = temp[2];
      left_[1] = temp[1];
      left_[2] = temp[0];
      temp[0] = up_[0];
      temp[1] = up_[1];
      up_[0] = up_[2];
      up_[2] = up_[7];
      up_[7] = up_[5];
      up_[5] = temp[0];
      up_[1] = up_[4];
      up_[4] = up_[6];
      up_[6] = up_[3];
      up_[3] = temp[1];
      break;
    case RubikCube::D_prime:
      temp[0] = back_[0];
      temp[1] = back_[1];
      temp[2] = back_[2];
      back_[0] = left_[7];
      back_[1] = left_[6];
      back_[2] = left_[5];
      left_[5] = front_[5];
      left_[6] = front_[6];
      left_[7] = front_[7];
      front_[5] = right_[5];
      front_[6] = right_[6];
      front_[7] = right_[7];
      right_[5] = temp[2];
      right_[6] = temp[1];
      right_[7] = temp[0];
      temp[0] = down_[0];
      temp[1] = down_[1];
      down_[0] = down_[2];
      down_[2] = down_[7];
      down_[7] = down_[5];
      down_[5] = temp[0];
      down_[1] = down_[4];
      down_[4] = down_[6];
      down_[6] = down_[3];
      down_[3] = temp[1];
      break;
    case RubikCube::F_prime:
      temp[0] = down_[0];
      temp[1] = down_[1];
      temp[2] = down_[2];
      down_[0] = left_[2];
      down_[1] = left_[4];
      down_[2] = left_[7];
      left_[2] = up_[7];
      left_[4] = up_[6];
      left_[7] = up_[5];
      up_[7] = right_[5];
      up_[6] = right_[3];
      up_[5] = right_[0];
      right_[5] = temp[0];
      right_[3] = temp[1];
      right_[0] = temp[2];
      temp[0] = front_[0];
      temp[1] = front_[1];
      front_[0] = front_[2];
      front_[2] = front_[7];
      front_[7] = front_[5];
      front_[5] = temp[0];
      front_[1] = front_[4];
      front_[4] = front_[6];
      front_[6] = front_[3];
      front_[3] = temp[1];
      break;
    case RubikCube::B_prime:
      temp[0] = up_[0];
      temp[1] = up_[1];
      temp[2] = up_[2];
      up_[0] = left_[5];
      up_[1] = left_[3];
      up_[2] = left_[0];
      left_[5] = down_[7];
      left_[3] = down_[6];
      left_[0] = down_[5];
      down_[7] = right_[2];
      down_[6] = right_[4];
      down_[5] = right_[7];
      right_[2] = temp[0];
      right_[4] = temp[1];
      right_[7] = temp[2];
      temp[0] = back_[0];
      temp[1] = back_[1];
      back_[0] = back_[2];
      back_[2] = back_[7];
      back_[7] = back_[5];
      back_[5] = temp[0];
      back_[1] = back_[4];
      back_[4] = back_[6];
      back_[6] = back_[3];
      back_[3] = temp[1];
      break;
    default:
      break;
  }
}

bool RubikCube::operator==(const RubikCube &y) const {
  return 0 == memcmp(y.getCube(), this->overall_, sizeof(overall_));
}
RubikCube &RubikCube::operator=(const RubikCube &y) {
  if (this != &y) {
    memcpy(overall_, y.getCube(), sizeof(overall_));
  }
  return *this;
}
