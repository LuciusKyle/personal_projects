#include"rubik_cube.h"

#include <immintrin.h>  // AVX2
#include <memory.h>     // for memcpy().
#include <algorithm>

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
	RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White,
	RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow,
	RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red,
	RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange,
	RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green,
	RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue
};

RubikCube::RubikCube(const char *ptr, const char *steps, const size_t step_count)
	: overall_{ ptr[0],  ptr[1],  ptr[2],  ptr[3],  ptr[4],  ptr[5],  ptr[6], ptr[7],  ptr[8],  ptr[9],  ptr[10], ptr[11], ptr[12], ptr[13], ptr[14], ptr[15], ptr[16], ptr[17], ptr[18], ptr[19], ptr[20], ptr[21], ptr[22], ptr[23], ptr[24], ptr[25], ptr[26], ptr[27], ptr[28], ptr[29], ptr[30], ptr[31], ptr[32], ptr[33], ptr[34], ptr[35], ptr[36], ptr[37], ptr[38], ptr[39], ptr[40], ptr[41], ptr[42], ptr[43], ptr[44], ptr[45], ptr[46], ptr[47] },
	up_(overall_ + 8 * 0),
	down_(overall_ + 8 * 1),
	front_(overall_ + 8 * 2),
	back_(overall_ + 8 * 3),
	left_(overall_ + 8 * 4),
	right_(overall_ + 8 * 5),
	steps_{steps[0], steps[1], steps[2], steps[3], steps[4], steps[5], steps[6], steps[7], steps[8], steps[9], steps[10], steps[11], steps[12], steps[13], steps[14]},
	step_count_(static_cast<char>(step_count)) {}

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
	: overall_{ static_cast<char>(up_0),   static_cast<char>(up_1),    static_cast<char>(up_2),    static_cast<char>(up_3),    static_cast<char>(up_4),    static_cast<char>(up_5),    static_cast<char>(up_6),
			   static_cast<char>(up_7),    static_cast<char>(down_0),  static_cast<char>(down_1),  static_cast<char>(down_2),  static_cast<char>(down_3),  static_cast<char>(down_4),  static_cast<char>(down_5),
			   static_cast<char>(down_6),  static_cast<char>(down_7),  static_cast<char>(front_0), static_cast<char>(front_1), static_cast<char>(front_2), static_cast<char>(front_3), static_cast<char>(front_4),
			   static_cast<char>(front_5), static_cast<char>(front_6), static_cast<char>(front_7), static_cast<char>(back_0),  static_cast<char>(back_1),  static_cast<char>(back_2),  static_cast<char>(back_3),
			   static_cast<char>(back_4),  static_cast<char>(back_5),  static_cast<char>(back_6),  static_cast<char>(back_7),  static_cast<char>(left_0),  static_cast<char>(left_1),  static_cast<char>(left_2),
			   static_cast<char>(left_3),  static_cast<char>(left_4),  static_cast<char>(left_5),  static_cast<char>(left_6),  static_cast<char>(left_7),  static_cast<char>(right_0), static_cast<char>(right_1),
			   static_cast<char>(right_2), static_cast<char>(right_3), static_cast<char>(right_4), static_cast<char>(right_5), static_cast<char>(right_6), static_cast<char>(right_7) },
	up_(overall_ + 8 * 0),
	down_(overall_ + 8 * 1),
	front_(overall_ + 8 * 2),
	back_(overall_ + 8 * 3),
	left_(overall_ + 8 * 4),
	right_(overall_ + 8 * 5),
	steps_{ 0 },
	step_count_(0) {}

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

const RubikCube *RubikCube::transform(CubeTransforms t) {
  using std::swap;
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
    case RubikCube::double_L:
      swap(up_[0], down_[0]);
      swap(up_[3], down_[3]);
      swap(up_[5], down_[5]);
      swap(front_[0], back_[0]);
      swap(front_[3], back_[3]);
      swap(front_[5], back_[5]);
      swap(left_[0], left_[7]);
      swap(left_[2], left_[5]);
      swap(left_[1], left_[6]);
      swap(left_[3], left_[4]);
      break;
    case RubikCube::double_R:
      swap(up_[2], down_[2]);
      swap(up_[4], down_[4]);
      swap(up_[7], down_[7]);
      swap(front_[2], back_[2]);
      swap(front_[4], back_[4]);
      swap(front_[7], back_[7]);
      swap(right_[0], right_[7]);
      swap(right_[2], right_[5]);
      swap(right_[1], right_[6]);
      swap(right_[3], right_[4]);
      break;
    case RubikCube::double_U:
      swap(front_[0], back_[7]);
      swap(front_[1], back_[6]);
      swap(front_[2], back_[5]);
      swap(left_[0], right_[0]);
      swap(left_[1], right_[1]);
      swap(left_[2], right_[2]);
      swap(up_[0], up_[7]);
      swap(up_[2], up_[5]);
      swap(up_[1], up_[6]);
      swap(up_[3], up_[4]);
      break;
    case RubikCube::double_D:
      swap(front_[7], back_[0]);
      swap(front_[6], back_[1]);
      swap(front_[5], back_[2]);
      swap(left_[5], right_[5]);
      swap(left_[6], right_[6]);
      swap(left_[7], right_[7]);
      swap(down_[0], down_[7]);
      swap(down_[2], down_[5]);
      swap(down_[1], down_[6]);
      swap(down_[3], down_[4]);
      break;
    case RubikCube::double_F:
      swap(up_[7], down_[0]);
      swap(up_[6], down_[1]);
      swap(up_[5], down_[2]);
      swap(left_[2], right_[5]);
      swap(left_[4], right_[3]);
      swap(left_[7], right_[0]);
      swap(front_[0], front_[7]);
      swap(front_[2], front_[5]);
      swap(front_[1], front_[6]);
      swap(front_[3], front_[4]);
      break;
    case RubikCube::double_B:
      swap(up_[0], down_[7]);
      swap(up_[1], down_[6]);
      swap(up_[2], down_[5]);
      swap(left_[5], right_[2]);
      swap(left_[3], right_[4]);
      swap(left_[0], right_[7]);
      swap(front_[0], front_[7]);
      swap(front_[2], front_[5]);
      swap(front_[1], front_[6]);
      swap(front_[3], front_[4]);
      break;
    default:
      break;
  }
  steps_[step_count_] = static_cast<char>(t);
  ++step_count_;
  return this;
}

bool RubikCube::operator==(const RubikCube &y) const {
	return 0 == memcmp(y.getCube(), this->overall_, sizeof(overall_));
}
RubikCube &RubikCube::operator=(const RubikCube &y) {
	if (this != &y) {
		memcpy(steps_, y.getSteps(), sizeof(steps_));
		memcpy(overall_, y.getCube(), sizeof(overall_));
    step_count_ = y.step_count_;
	}
	return *this;
}

void RubikCube::resetStep() {
	memset(steps_, 0, sizeof(steps_));
	step_count_ = 0;
}