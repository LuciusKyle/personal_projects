#include"rubik_cube.h"

#include <immintrin.h>  // AVX2
#include <memory.h>     // for memcpy().
#include <algorithm>

//#define USE_AVX2

constexpr char solved_cube[48] = {
	RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White, RubikCube::Colors::White,
	RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow, RubikCube::Colors::Yellow,
	RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red,
	RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange, RubikCube::Colors::Orange,
	RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green, RubikCube::Colors::Green,
	RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue
};
const __m256i kSolvedFrontBackLeftRight = _mm256_load_si256(reinterpret_cast<const __m256i *>(solved_cube + 16));
const __m128i kSolvedUpAndDown = _mm_load_si128(reinterpret_cast<const __m128i *>(solved_cube));

RubikCube::RubikCube(const char *ptr, const char *steps, const size_t step_count)
	: overall_{ ptr[0],  ptr[1],  ptr[2],  ptr[3],  ptr[4],  ptr[5],  ptr[6], ptr[7],  ptr[8],  ptr[9],  ptr[10], ptr[11], ptr[12], ptr[13], ptr[14], ptr[15], ptr[16], ptr[17], ptr[18], ptr[19], ptr[20], ptr[21], ptr[22], ptr[23], ptr[24], ptr[25], ptr[26], ptr[27], ptr[28], ptr[29], ptr[30], ptr[31], ptr[32], ptr[33], ptr[34], ptr[35], ptr[36], ptr[37], ptr[38], ptr[39], ptr[40], ptr[41], ptr[42], ptr[43], ptr[44], ptr[45], ptr[46], ptr[47] },
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
	steps_{ 0 },
	step_count_(0) {}

bool RubikCube::isSolved() const {
#ifdef USE_AVX2
	return (
		0xffff ==
		_mm_movemask_epi8(_mm_cmpeq_epi8(
			_mm_load_si128(reinterpret_cast<const __m128i *>(overall_)), kSolvedUpAndDown))
		&&
		int(0xffffffff) ==
		_mm256_movemask_epi8(_mm256_cmpeq_epi8(
			_mm256_load_si256(reinterpret_cast<const __m256i *>(overall_ + 16)), kSolvedFrontBackLeftRight))
		);
#else
	return 0 == memcmp(solved_cube, overall_, sizeof(overall_));
#endif  // USE_AVX2
}

const RubikCube *RubikCube::transform(CubeTransforms t) {
  using std::swap;
  char temp[3] = {0};
  switch (t) {
    case RubikCube::L:
      temp[0] = overall_[0 * 8 + 0];
      temp[1] = overall_[0 * 8 + 3];
      temp[2] = overall_[0 * 8 + 5];
      overall_[0 * 8 + 0] = overall_[3 * 8 + 0];
      overall_[0 * 8 + 3] = overall_[3 * 8 + 3];
      overall_[0 * 8 + 5] = overall_[3 * 8 + 5];
      overall_[3 * 8 + 0] = overall_[1 * 8 + 0];
      overall_[3 * 8 + 3] = overall_[1 * 8 + 3];
      overall_[3 * 8 + 5] = overall_[1 * 8 + 5];
      overall_[1 * 8 + 0] = overall_[2 * 8 + 0];
      overall_[1 * 8 + 3] = overall_[2 * 8 + 3];
      overall_[1 * 8 + 5] = overall_[2 * 8 + 5];
      overall_[2 * 8 + 0] = temp[0];
      overall_[2 * 8 + 3] = temp[1];
      overall_[2 * 8 + 5] = temp[2];
      temp[0] = overall_[4 * 8 + 0];
      temp[1] = overall_[4 * 8 + 1];
      overall_[4 * 8 + 0] = overall_[4 * 8 + 5];
      overall_[4 * 8 + 5] = overall_[4 * 8 + 7];
      overall_[4 * 8 + 7] = overall_[4 * 8 + 2];
      overall_[4 * 8 + 2] = temp[0];
      overall_[4 * 8 + 1] = overall_[4 * 8 + 3];
      overall_[4 * 8 + 3] = overall_[4 * 8 + 6];
      overall_[4 * 8 + 6] = overall_[4 * 8 + 4];
      overall_[4 * 8 + 4] = temp[1];
      break;
    case RubikCube::R:
      temp[0] = overall_[0 * 8 + 2];
      temp[1] = overall_[0 * 8 + 4];
      temp[2] = overall_[0 * 8 + 7];
      overall_[0 * 8 + 2] = overall_[2 * 8 + 2];
      overall_[0 * 8 + 4] = overall_[2 * 8 + 4];
      overall_[0 * 8 + 7] = overall_[2 * 8 + 7];
      overall_[2 * 8 + 2] = overall_[1 * 8 + 2];
      overall_[2 * 8 + 4] = overall_[1 * 8 + 4];
      overall_[2 * 8 + 7] = overall_[1 * 8 + 7];
      overall_[1 * 8 + 2] = overall_[3 * 8 + 2];
      overall_[1 * 8 + 4] = overall_[3 * 8 + 4];
      overall_[1 * 8 + 7] = overall_[3 * 8 + 7];
      overall_[3 * 8 + 2] = temp[0];
      overall_[3 * 8 + 4] = temp[1];
      overall_[3 * 8 + 7] = temp[2];
      temp[0] = overall_[5 * 8 + 0];
      temp[1] = overall_[5 * 8 + 1];
      overall_[5 * 8 + 0] = overall_[5 * 8 + 5];
      overall_[5 * 8 + 5] = overall_[5 * 8 + 7];
      overall_[5 * 8 + 7] = overall_[5 * 8 + 2];
      overall_[5 * 8 + 2] = temp[0];
      overall_[5 * 8 + 1] = overall_[5 * 8 + 3];
      overall_[5 * 8 + 3] = overall_[5 * 8 + 6];
      overall_[5 * 8 + 6] = overall_[5 * 8 + 4];
      overall_[5 * 8 + 4] = temp[1];
      break;
    case RubikCube::U:
      temp[0] = overall_[3 * 8 + 5];
      temp[1] = overall_[3 * 8 + 6];
      temp[2] = overall_[3 * 8 + 7];
      overall_[3 * 8 + 5] = overall_[4 * 8 + 2];
      overall_[3 * 8 + 6] = overall_[4 * 8 + 1];
      overall_[3 * 8 + 7] = overall_[4 * 8 + 0];
      overall_[4 * 8 + 0] = overall_[2 * 8 + 0];
      overall_[4 * 8 + 1] = overall_[2 * 8 + 1];
      overall_[4 * 8 + 2] = overall_[2 * 8 + 2];
      overall_[2 * 8 + 0] = overall_[5 * 8 + 0];
      overall_[2 * 8 + 1] = overall_[5 * 8 + 1];
      overall_[2 * 8 + 2] = overall_[5 * 8 + 2];
      overall_[5 * 8 + 0] = temp[2];
      overall_[5 * 8 + 1] = temp[1];
      overall_[5 * 8 + 2] = temp[0];
      temp[0] = overall_[0 * 8 + 0];
      temp[1] = overall_[0 * 8 + 1];
      overall_[0 * 8 + 0] = overall_[0 * 8 + 5];
      overall_[0 * 8 + 5] = overall_[0 * 8 + 7];
      overall_[0 * 8 + 7] = overall_[0 * 8 + 2];
      overall_[0 * 8 + 2] = temp[0];
      overall_[0 * 8 + 1] = overall_[0 * 8 + 3];
      overall_[0 * 8 + 3] = overall_[0 * 8 + 6];
      overall_[0 * 8 + 6] = overall_[0 * 8 + 4];
      overall_[0 * 8 + 4] = temp[1];
      break;
    case RubikCube::D:
      temp[0] = overall_[3 * 8 + 0];
      temp[1] = overall_[3 * 8 + 1];
      temp[2] = overall_[3 * 8 + 2];
      overall_[3 * 8 + 0] = overall_[5 * 8 + 7];
      overall_[3 * 8 + 1] = overall_[5 * 8 + 6];
      overall_[3 * 8 + 2] = overall_[5 * 8 + 5];
      overall_[5 * 8 + 5] = overall_[2 * 8 + 5];
      overall_[5 * 8 + 6] = overall_[2 * 8 + 6];
      overall_[5 * 8 + 7] = overall_[2 * 8 + 7];
      overall_[2 * 8 + 5] = overall_[4 * 8 + 5];
      overall_[2 * 8 + 6] = overall_[4 * 8 + 6];
      overall_[2 * 8 + 7] = overall_[4 * 8 + 7];
      overall_[4 * 8 + 5] = temp[2];
      overall_[4 * 8 + 6] = temp[1];
      overall_[4 * 8 + 7] = temp[0];
      temp[0] = overall_[1 * 8 + 0];
      temp[1] = overall_[1 * 8 + 1];
      overall_[1 * 8 + 0] = overall_[1 * 8 + 5];
      overall_[1 * 8 + 5] = overall_[1 * 8 + 7];
      overall_[1 * 8 + 7] = overall_[1 * 8 + 2];
      overall_[1 * 8 + 2] = temp[0];
      overall_[1 * 8 + 1] = overall_[1 * 8 + 3];
      overall_[1 * 8 + 3] = overall_[1 * 8 + 6];
      overall_[1 * 8 + 6] = overall_[1 * 8 + 4];
      overall_[1 * 8 + 4] = temp[1];
      break;
    case RubikCube::F:
      temp[0] = overall_[1 * 8 + 0];
      temp[1] = overall_[1 * 8 + 1];
      temp[2] = overall_[1 * 8 + 2];
      overall_[1 * 8 + 0] = overall_[5 * 8 + 5];
      overall_[1 * 8 + 1] = overall_[5 * 8 + 3];
      overall_[1 * 8 + 2] = overall_[5 * 8 + 0];
      overall_[5 * 8 + 5] = overall_[0 * 8 + 7];
      overall_[5 * 8 + 3] = overall_[0 * 8 + 6];
      overall_[5 * 8 + 0] = overall_[0 * 8 + 5];
      overall_[0 * 8 + 7] = overall_[4 * 8 + 2];
      overall_[0 * 8 + 6] = overall_[4 * 8 + 4];
      overall_[0 * 8 + 5] = overall_[4 * 8 + 7];
      overall_[4 * 8 + 2] = temp[0];
      overall_[4 * 8 + 4] = temp[1];
      overall_[4 * 8 + 7] = temp[2];
      temp[0] = overall_[2 * 8 + 0];
      temp[1] = overall_[2 * 8 + 1];
      overall_[2 * 8 + 0] = overall_[2 * 8 + 5];
      overall_[2 * 8 + 5] = overall_[2 * 8 + 7];
      overall_[2 * 8 + 7] = overall_[2 * 8 + 2];
      overall_[2 * 8 + 2] = temp[0];
      overall_[2 * 8 + 1] = overall_[2 * 8 + 3];
      overall_[2 * 8 + 3] = overall_[2 * 8 + 6];
      overall_[2 * 8 + 6] = overall_[2 * 8 + 4];
      overall_[2 * 8 + 4] = temp[1];
      break;
    case RubikCube::B:
      temp[0] = overall_[0 * 8 + 0];
      temp[1] = overall_[0 * 8 + 1];
      temp[2] = overall_[0 * 8 + 2];
      overall_[0 * 8 + 0] = overall_[5 * 8 + 2];
      overall_[0 * 8 + 1] = overall_[5 * 8 + 4];
      overall_[0 * 8 + 2] = overall_[5 * 8 + 7];
      overall_[5 * 8 + 2] = overall_[1 * 8 + 7];
      overall_[5 * 8 + 4] = overall_[1 * 8 + 6];
      overall_[5 * 8 + 7] = overall_[1 * 8 + 5];
      overall_[1 * 8 + 7] = overall_[4 * 8 + 5];
      overall_[1 * 8 + 6] = overall_[4 * 8 + 3];
      overall_[1 * 8 + 5] = overall_[4 * 8 + 0];
      overall_[4 * 8 + 5] = temp[0];
      overall_[4 * 8 + 3] = temp[1];
      overall_[4 * 8 + 0] = temp[2];
      temp[0] = overall_[3 * 8 + 0];
      temp[1] = overall_[3 * 8 + 1];
      overall_[3 * 8 + 0] = overall_[3 * 8 + 5];
      overall_[3 * 8 + 5] = overall_[3 * 8 + 7];
      overall_[3 * 8 + 7] = overall_[3 * 8 + 2];
      overall_[3 * 8 + 2] = temp[0];
      overall_[3 * 8 + 1] = overall_[3 * 8 + 3];
      overall_[3 * 8 + 3] = overall_[3 * 8 + 6];
      overall_[3 * 8 + 6] = overall_[3 * 8 + 4];
      overall_[3 * 8 + 4] = temp[1];
      break;
    case RubikCube::L_prime:
      temp[0] = overall_[0 * 8 + 0];
      temp[1] = overall_[0 * 8 + 3];
      temp[2] = overall_[0 * 8 + 5];
      overall_[0 * 8 + 0] = overall_[2 * 8 + 0];
      overall_[0 * 8 + 3] = overall_[2 * 8 + 3];
      overall_[0 * 8 + 5] = overall_[2 * 8 + 5];
      overall_[2 * 8 + 0] = overall_[1 * 8 + 0];
      overall_[2 * 8 + 3] = overall_[1 * 8 + 3];
      overall_[2 * 8 + 5] = overall_[1 * 8 + 5];
      overall_[1 * 8 + 0] = overall_[3 * 8 + 0];
      overall_[1 * 8 + 3] = overall_[3 * 8 + 3];
      overall_[1 * 8 + 5] = overall_[3 * 8 + 5];
      overall_[3 * 8 + 0] = temp[0];
      overall_[3 * 8 + 3] = temp[1];
      overall_[3 * 8 + 5] = temp[2];
      temp[0] = overall_[4 * 8 + 0];
      temp[1] = overall_[4 * 8 + 1];
      overall_[4 * 8 + 0] = overall_[4 * 8 + 2];
      overall_[4 * 8 + 2] = overall_[4 * 8 + 7];
      overall_[4 * 8 + 7] = overall_[4 * 8 + 5];
      overall_[4 * 8 + 5] = temp[0];
      overall_[4 * 8 + 1] = overall_[4 * 8 + 4];
      overall_[4 * 8 + 4] = overall_[4 * 8 + 6];
      overall_[4 * 8 + 6] = overall_[4 * 8 + 3];
      overall_[4 * 8 + 3] = temp[1];
      break;
    case RubikCube::R_prime:
      temp[0] = overall_[0 * 8 + 2];
      temp[1] = overall_[0 * 8 + 4];
      temp[2] = overall_[0 * 8 + 7];
      overall_[0 * 8 + 2] = overall_[3 * 8 + 2];
      overall_[0 * 8 + 4] = overall_[3 * 8 + 4];
      overall_[0 * 8 + 7] = overall_[3 * 8 + 7];
      overall_[3 * 8 + 2] = overall_[1 * 8 + 2];
      overall_[3 * 8 + 4] = overall_[1 * 8 + 4];
      overall_[3 * 8 + 7] = overall_[1 * 8 + 7];
      overall_[1 * 8 + 2] = overall_[2 * 8 + 2];
      overall_[1 * 8 + 4] = overall_[2 * 8 + 4];
      overall_[1 * 8 + 7] = overall_[2 * 8 + 7];
      overall_[2 * 8 + 2] = temp[0];
      overall_[2 * 8 + 4] = temp[1];
      overall_[2 * 8 + 7] = temp[2];
      temp[0] = overall_[5 * 8 + 0];
      temp[1] = overall_[5 * 8 + 1];
      overall_[5 * 8 + 0] = overall_[5 * 8 + 2];
      overall_[5 * 8 + 2] = overall_[5 * 8 + 7];
      overall_[5 * 8 + 7] = overall_[5 * 8 + 5];
      overall_[5 * 8 + 5] = temp[0];
      overall_[5 * 8 + 1] = overall_[5 * 8 + 4];
      overall_[5 * 8 + 4] = overall_[5 * 8 + 6];
      overall_[5 * 8 + 6] = overall_[5 * 8 + 3];
      overall_[5 * 8 + 3] = temp[1];
      break;
    case RubikCube::U_prime:
      temp[0] = overall_[3 * 8 + 5];
      temp[1] = overall_[3 * 8 + 6];
      temp[2] = overall_[3 * 8 + 7];
      overall_[3 * 8 + 5] = overall_[5 * 8 + 2];
      overall_[3 * 8 + 6] = overall_[5 * 8 + 1];
      overall_[3 * 8 + 7] = overall_[5 * 8 + 0];
      overall_[5 * 8 + 0] = overall_[2 * 8 + 0];
      overall_[5 * 8 + 1] = overall_[2 * 8 + 1];
      overall_[5 * 8 + 2] = overall_[2 * 8 + 2];
      overall_[2 * 8 + 0] = overall_[4 * 8 + 0];
      overall_[2 * 8 + 1] = overall_[4 * 8 + 1];
      overall_[2 * 8 + 2] = overall_[4 * 8 + 2];
      overall_[4 * 8 + 0] = temp[2];
      overall_[4 * 8 + 1] = temp[1];
      overall_[4 * 8 + 2] = temp[0];
      temp[0] = overall_[0 * 8 + 0];
      temp[1] = overall_[0 * 8 + 1];
      overall_[0 * 8 + 0] = overall_[0 * 8 + 2];
      overall_[0 * 8 + 2] = overall_[0 * 8 + 7];
      overall_[0 * 8 + 7] = overall_[0 * 8 + 5];
      overall_[0 * 8 + 5] = temp[0];
      overall_[0 * 8 + 1] = overall_[0 * 8 + 4];
      overall_[0 * 8 + 4] = overall_[0 * 8 + 6];
      overall_[0 * 8 + 6] = overall_[0 * 8 + 3];
      overall_[0 * 8 + 3] = temp[1];
      break;
    case RubikCube::D_prime:
      temp[0] = overall_[3 * 8 + 0];
      temp[1] = overall_[3 * 8 + 1];
      temp[2] = overall_[3 * 8 + 2];
      overall_[3 * 8 + 0] = overall_[4 * 8 + 7];
      overall_[3 * 8 + 1] = overall_[4 * 8 + 6];
      overall_[3 * 8 + 2] = overall_[4 * 8 + 5];
      overall_[4 * 8 + 5] = overall_[2 * 8 + 5];
      overall_[4 * 8 + 6] = overall_[2 * 8 + 6];
      overall_[4 * 8 + 7] = overall_[2 * 8 + 7];
      overall_[2 * 8 + 5] = overall_[5 * 8 + 5];
      overall_[2 * 8 + 6] = overall_[5 * 8 + 6];
      overall_[2 * 8 + 7] = overall_[5 * 8 + 7];
      overall_[5 * 8 + 5] = temp[2];
      overall_[5 * 8 + 6] = temp[1];
      overall_[5 * 8 + 7] = temp[0];
      temp[0] = overall_[1 * 8 + 0];
      temp[1] = overall_[1 * 8 + 1];
      overall_[1 * 8 + 0] = overall_[1 * 8 + 2];
      overall_[1 * 8 + 2] = overall_[1 * 8 + 7];
      overall_[1 * 8 + 7] = overall_[1 * 8 + 5];
      overall_[1 * 8 + 5] = temp[0];
      overall_[1 * 8 + 1] = overall_[1 * 8 + 4];
      overall_[1 * 8 + 4] = overall_[1 * 8 + 6];
      overall_[1 * 8 + 6] = overall_[1 * 8 + 3];
      overall_[1 * 8 + 3] = temp[1];
      break;
    case RubikCube::F_prime:
      temp[0] = overall_[1 * 8 + 0];
      temp[1] = overall_[1 * 8 + 1];
      temp[2] = overall_[1 * 8 + 2];
      overall_[1 * 8 + 0] = overall_[4 * 8 + 2];
      overall_[1 * 8 + 1] = overall_[4 * 8 + 4];
      overall_[1 * 8 + 2] = overall_[4 * 8 + 7];
      overall_[4 * 8 + 2] = overall_[0 * 8 + 7];
      overall_[4 * 8 + 4] = overall_[0 * 8 + 6];
      overall_[4 * 8 + 7] = overall_[0 * 8 + 5];
      overall_[0 * 8 + 7] = overall_[5 * 8 + 5];
      overall_[0 * 8 + 6] = overall_[5 * 8 + 3];
      overall_[0 * 8 + 5] = overall_[5 * 8 + 0];
      overall_[5 * 8 + 5] = temp[0];
      overall_[5 * 8 + 3] = temp[1];
      overall_[5 * 8 + 0] = temp[2];
      temp[0] = overall_[2 * 8 + 0];
      temp[1] = overall_[2 * 8 + 1];
      overall_[2 * 8 + 0] = overall_[2 * 8 + 2];
      overall_[2 * 8 + 2] = overall_[2 * 8 + 7];
      overall_[2 * 8 + 7] = overall_[2 * 8 + 5];
      overall_[2 * 8 + 5] = temp[0];
      overall_[2 * 8 + 1] = overall_[2 * 8 + 4];
      overall_[2 * 8 + 4] = overall_[2 * 8 + 6];
      overall_[2 * 8 + 6] = overall_[2 * 8 + 3];
      overall_[2 * 8 + 3] = temp[1];
      break;
    case RubikCube::B_prime:
      temp[0] = overall_[0 * 8 + 0];
      temp[1] = overall_[0 * 8 + 1];
      temp[2] = overall_[0 * 8 + 2];
      overall_[0 * 8 + 0] = overall_[4 * 8 + 5];
      overall_[0 * 8 + 1] = overall_[4 * 8 + 3];
      overall_[0 * 8 + 2] = overall_[4 * 8 + 0];
      overall_[4 * 8 + 5] = overall_[1 * 8 + 7];
      overall_[4 * 8 + 3] = overall_[1 * 8 + 6];
      overall_[4 * 8 + 0] = overall_[1 * 8 + 5];
      overall_[1 * 8 + 7] = overall_[5 * 8 + 2];
      overall_[1 * 8 + 6] = overall_[5 * 8 + 4];
      overall_[1 * 8 + 5] = overall_[5 * 8 + 7];
      overall_[5 * 8 + 2] = temp[0];
      overall_[5 * 8 + 4] = temp[1];
      overall_[5 * 8 + 7] = temp[2];
      temp[0] = overall_[3 * 8 + 0];
      temp[1] = overall_[3 * 8 + 1];
      overall_[3 * 8 + 0] = overall_[3 * 8 + 2];
      overall_[3 * 8 + 2] = overall_[3 * 8 + 7];
      overall_[3 * 8 + 7] = overall_[3 * 8 + 5];
      overall_[3 * 8 + 5] = temp[0];
      overall_[3 * 8 + 1] = overall_[3 * 8 + 4];
      overall_[3 * 8 + 4] = overall_[3 * 8 + 6];
      overall_[3 * 8 + 6] = overall_[3 * 8 + 3];
      overall_[3 * 8 + 3] = temp[1];
      break;
    case RubikCube::double_L:
      swap(overall_[0 * 8 + 0], overall_[1 * 8 + 0]);
      swap(overall_[0 * 8 + 3], overall_[1 * 8 + 3]);
      swap(overall_[0 * 8 + 5], overall_[1 * 8 + 5]);
      swap(overall_[2 * 8 + 0], overall_[3 * 8 + 0]);
      swap(overall_[2 * 8 + 3], overall_[3 * 8 + 3]);
      swap(overall_[2 * 8 + 5], overall_[3 * 8 + 5]);
      swap(overall_[4 * 8 + 0], overall_[4 * 8 + 7]);
      swap(overall_[4 * 8 + 2], overall_[4 * 8 + 5]);
      swap(overall_[4 * 8 + 1], overall_[4 * 8 + 6]);
      swap(overall_[4 * 8 + 3], overall_[4 * 8 + 4]);
      break;
    case RubikCube::double_R:
      swap(overall_[0 * 8 + 2], overall_[1 * 8 + 2]);
      swap(overall_[0 * 8 + 4], overall_[1 * 8 + 4]);
      swap(overall_[0 * 8 + 7], overall_[1 * 8 + 7]);
      swap(overall_[2 * 8 + 2], overall_[3 * 8 + 2]);
      swap(overall_[2 * 8 + 4], overall_[3 * 8 + 4]);
      swap(overall_[2 * 8 + 7], overall_[3 * 8 + 7]);
      swap(overall_[5 * 8 + 0], overall_[5 * 8 + 7]);
      swap(overall_[5 * 8 + 2], overall_[5 * 8 + 5]);
      swap(overall_[5 * 8 + 1], overall_[5 * 8 + 6]);
      swap(overall_[5 * 8 + 3], overall_[5 * 8 + 4]);
      break;
    case RubikCube::double_U:
      swap(overall_[2 * 8 + 0], overall_[3 * 8 + 7]);
      swap(overall_[2 * 8 + 1], overall_[3 * 8 + 6]);
      swap(overall_[2 * 8 + 2], overall_[3 * 8 + 5]);
      swap(overall_[4 * 8 + 0], overall_[5 * 8 + 0]);
      swap(overall_[4 * 8 + 1], overall_[5 * 8 + 1]);
      swap(overall_[4 * 8 + 2], overall_[5 * 8 + 2]);
      swap(overall_[0 * 8 + 0], overall_[0 * 8 + 7]);
      swap(overall_[0 * 8 + 2], overall_[0 * 8 + 5]);
      swap(overall_[0 * 8 + 1], overall_[0 * 8 + 6]);
      swap(overall_[0 * 8 + 3], overall_[0 * 8 + 4]);
      break;
    case RubikCube::double_D:
      swap(overall_[2 * 8 + 7], overall_[3 * 8 + 0]);
      swap(overall_[2 * 8 + 6], overall_[3 * 8 + 1]);
      swap(overall_[2 * 8 + 5], overall_[3 * 8 + 2]);
      swap(overall_[4 * 8 + 5], overall_[5 * 8 + 5]);
      swap(overall_[4 * 8 + 6], overall_[5 * 8 + 6]);
      swap(overall_[4 * 8 + 7], overall_[5 * 8 + 7]);
      swap(overall_[1 * 8 + 0], overall_[1 * 8 + 7]);
      swap(overall_[1 * 8 + 2], overall_[1 * 8 + 5]);
      swap(overall_[1 * 8 + 1], overall_[1 * 8 + 6]);
      swap(overall_[1 * 8 + 3], overall_[1 * 8 + 4]);
      break;
    case RubikCube::double_F:
      swap(overall_[0 * 8 + 7], overall_[1 * 8 + 0]);
      swap(overall_[0 * 8 + 6], overall_[1 * 8 + 1]);
      swap(overall_[0 * 8 + 5], overall_[1 * 8 + 2]);
      swap(overall_[4 * 8 + 2], overall_[5 * 8 + 5]);
      swap(overall_[4 * 8 + 4], overall_[5 * 8 + 3]);
      swap(overall_[4 * 8 + 7], overall_[5 * 8 + 0]);
      swap(overall_[2 * 8 + 0], overall_[2 * 8 + 7]);
      swap(overall_[2 * 8 + 2], overall_[2 * 8 + 5]);
      swap(overall_[2 * 8 + 1], overall_[2 * 8 + 6]);
      swap(overall_[2 * 8 + 3], overall_[2 * 8 + 4]);
      break;
    case RubikCube::double_B:
      swap(overall_[0 * 8 + 0], overall_[1 * 8 + 7]);
      swap(overall_[0 * 8 + 1], overall_[1 * 8 + 6]);
      swap(overall_[0 * 8 + 2], overall_[1 * 8 + 5]);
      swap(overall_[4 * 8 + 5], overall_[5 * 8 + 2]);
      swap(overall_[4 * 8 + 3], overall_[5 * 8 + 4]);
      swap(overall_[4 * 8 + 0], overall_[5 * 8 + 7]);
      swap(overall_[3 * 8 + 0], overall_[3 * 8 + 7]);
      swap(overall_[3 * 8 + 2], overall_[3 * 8 + 5]);
      swap(overall_[3 * 8 + 1], overall_[3 * 8 + 6]);
      swap(overall_[3 * 8 + 3], overall_[3 * 8 + 4]);
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