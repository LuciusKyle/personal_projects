
#include "rubik_cube.h"

#include <immintrin.h>
#include<tbb/tbb.h>
#include <array>
#include <iostream>
#include <string>

using std::array;
using std::cout;
using std::endl;
using std::string;

constexpr size_t kCubeSize = 48;
constexpr unsigned int kMaxSteps = 8;

const RubikCube kInitialCube(
	RubikCube::Colors::White, RubikCube::Colors::White,
	RubikCube::Colors::White, RubikCube::Colors::White,
	RubikCube::Colors::White, RubikCube::Colors::White,
	RubikCube::Colors::White, RubikCube::Colors::White,
	RubikCube::Colors::Yellow, RubikCube::Colors::Yellow,
	RubikCube::Colors::Yellow, RubikCube::Colors::Yellow,
	RubikCube::Colors::Yellow, RubikCube::Colors::Yellow,
	RubikCube::Colors::Yellow, RubikCube::Colors::Yellow,
	RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red,
	RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Red,
	RubikCube::Colors::Red, RubikCube::Colors::Red, RubikCube::Colors::Orange,
	RubikCube::Colors::Orange, RubikCube::Colors::Orange,
	RubikCube::Colors::Orange, RubikCube::Colors::Orange,
	RubikCube::Colors::Orange, RubikCube::Colors::Orange,
	RubikCube::Colors::Orange, RubikCube::Colors::Green,
	RubikCube::Colors::Green, RubikCube::Colors::Green,
	RubikCube::Colors::Green, RubikCube::Colors::Green,
	RubikCube::Colors::Green, RubikCube::Colors::Green,
	RubikCube::Colors::Green, RubikCube::Colors::Blue, RubikCube::Colors::Blue,
	RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue,
	RubikCube::Colors::Blue, RubikCube::Colors::Blue, RubikCube::Colors::Blue);

constexpr int kReverseTransform = 11;
constexpr array kColorsDict{ 'R', 'O', 'G', 'B', 'Y', 'W' };
constexpr array kCubeTransforms{
    RubikCube::CubeTransforms::L,        RubikCube::CubeTransforms::R,
    RubikCube::CubeTransforms::U,        RubikCube::CubeTransforms::D,
    RubikCube::CubeTransforms::F,        RubikCube::CubeTransforms::B,
    RubikCube::CubeTransforms::L_prime,  RubikCube::CubeTransforms::R_prime,
    RubikCube::CubeTransforms::U_prime,  RubikCube::CubeTransforms::D_prime,
    RubikCube::CubeTransforms::F_prime,  RubikCube::CubeTransforms::B_prime,
    RubikCube::CubeTransforms::double_L, RubikCube::CubeTransforms::double_R,
    RubikCube::CubeTransforms::double_U, RubikCube::CubeTransforms::double_D,
    RubikCube::CubeTransforms::double_F, RubikCube::CubeTransforms::double_B};
const array<string, 18> kCubeTransformsDict{ "L", "R", "U", "D", "F", "B", "B'", "F'", "D'", "U'", "R'", "L'","L2", "R2", "U2", "D2", "F2", "B2" };

constexpr array kAvailableTransforms{
  array{RubikCube::CubeTransforms::R,RubikCube::CubeTransforms::U,RubikCube::CubeTransforms::D,RubikCube::CubeTransforms::F,RubikCube::CubeTransforms::B,RubikCube::CubeTransforms::B_prime,RubikCube::CubeTransforms::F_prime,RubikCube::CubeTransforms::D_prime,RubikCube::CubeTransforms::U_prime,RubikCube::CubeTransforms::R_prime,RubikCube::CubeTransforms::double_R,RubikCube::CubeTransforms::double_U,RubikCube::CubeTransforms::double_D,RubikCube::CubeTransforms::double_F,RubikCube::CubeTransforms::double_B},
  array{RubikCube::CubeTransforms::L,RubikCube::CubeTransforms::U,RubikCube::CubeTransforms::D,RubikCube::CubeTransforms::F,RubikCube::CubeTransforms::B,RubikCube::CubeTransforms::B_prime,RubikCube::CubeTransforms::F_prime,RubikCube::CubeTransforms::D_prime,RubikCube::CubeTransforms::U_prime,RubikCube::CubeTransforms::L_prime,RubikCube::CubeTransforms::double_L,RubikCube::CubeTransforms::double_U,RubikCube::CubeTransforms::double_D,RubikCube::CubeTransforms::double_F,RubikCube::CubeTransforms::double_B},
  array{RubikCube::CubeTransforms::L,RubikCube::CubeTransforms::R,RubikCube::CubeTransforms::D,RubikCube::CubeTransforms::F,RubikCube::CubeTransforms::B,RubikCube::CubeTransforms::B_prime,RubikCube::CubeTransforms::F_prime,RubikCube::CubeTransforms::D_prime,RubikCube::CubeTransforms::R_prime,RubikCube::CubeTransforms::L_prime,RubikCube::CubeTransforms::double_L,RubikCube::CubeTransforms::double_R,RubikCube::CubeTransforms::double_D,RubikCube::CubeTransforms::double_F,RubikCube::CubeTransforms::double_B},
  array{RubikCube::CubeTransforms::L,RubikCube::CubeTransforms::R,RubikCube::CubeTransforms::U,RubikCube::CubeTransforms::F,RubikCube::CubeTransforms::B,RubikCube::CubeTransforms::B_prime,RubikCube::CubeTransforms::F_prime,RubikCube::CubeTransforms::U_prime,RubikCube::CubeTransforms::R_prime,RubikCube::CubeTransforms::L_prime,RubikCube::CubeTransforms::double_L,RubikCube::CubeTransforms::double_R,RubikCube::CubeTransforms::double_U,RubikCube::CubeTransforms::double_F,RubikCube::CubeTransforms::double_B},
  array{RubikCube::CubeTransforms::L,RubikCube::CubeTransforms::R,RubikCube::CubeTransforms::U,RubikCube::CubeTransforms::D,RubikCube::CubeTransforms::B,RubikCube::CubeTransforms::B_prime,RubikCube::CubeTransforms::D_prime,RubikCube::CubeTransforms::U_prime,RubikCube::CubeTransforms::R_prime,RubikCube::CubeTransforms::L_prime,RubikCube::CubeTransforms::double_L,RubikCube::CubeTransforms::double_R,RubikCube::CubeTransforms::double_U,RubikCube::CubeTransforms::double_D,RubikCube::CubeTransforms::double_B},
  array{RubikCube::CubeTransforms::L,RubikCube::CubeTransforms::R,RubikCube::CubeTransforms::U,RubikCube::CubeTransforms::D,RubikCube::CubeTransforms::F,RubikCube::CubeTransforms::F_prime,RubikCube::CubeTransforms::D_prime,RubikCube::CubeTransforms::U_prime,RubikCube::CubeTransforms::R_prime,RubikCube::CubeTransforms::L_prime,RubikCube::CubeTransforms::double_L,RubikCube::CubeTransforms::double_R,RubikCube::CubeTransforms::double_U,RubikCube::CubeTransforms::double_D,RubikCube::CubeTransforms::double_F},
  array{RubikCube::CubeTransforms::L,RubikCube::CubeTransforms::R,RubikCube::CubeTransforms::U,RubikCube::CubeTransforms::D,RubikCube::CubeTransforms::F,RubikCube::CubeTransforms::F_prime,RubikCube::CubeTransforms::D_prime,RubikCube::CubeTransforms::U_prime,RubikCube::CubeTransforms::R_prime,RubikCube::CubeTransforms::L_prime,RubikCube::CubeTransforms::double_L,RubikCube::CubeTransforms::double_R,RubikCube::CubeTransforms::double_U,RubikCube::CubeTransforms::double_D,RubikCube::CubeTransforms::double_F},
  array{RubikCube::CubeTransforms::L,RubikCube::CubeTransforms::R,RubikCube::CubeTransforms::U,RubikCube::CubeTransforms::D,RubikCube::CubeTransforms::B,RubikCube::CubeTransforms::B_prime,RubikCube::CubeTransforms::D_prime,RubikCube::CubeTransforms::U_prime,RubikCube::CubeTransforms::R_prime,RubikCube::CubeTransforms::L_prime,RubikCube::CubeTransforms::double_L,RubikCube::CubeTransforms::double_R,RubikCube::CubeTransforms::double_U,RubikCube::CubeTransforms::double_D,RubikCube::CubeTransforms::double_B},
  array{RubikCube::CubeTransforms::L,RubikCube::CubeTransforms::R,RubikCube::CubeTransforms::U,RubikCube::CubeTransforms::F,RubikCube::CubeTransforms::B,RubikCube::CubeTransforms::B_prime,RubikCube::CubeTransforms::F_prime,RubikCube::CubeTransforms::U_prime,RubikCube::CubeTransforms::R_prime,RubikCube::CubeTransforms::L_prime,RubikCube::CubeTransforms::double_L,RubikCube::CubeTransforms::double_R,RubikCube::CubeTransforms::double_U,RubikCube::CubeTransforms::double_F,RubikCube::CubeTransforms::double_B},
  array{RubikCube::CubeTransforms::L,RubikCube::CubeTransforms::R,RubikCube::CubeTransforms::D,RubikCube::CubeTransforms::F,RubikCube::CubeTransforms::B,RubikCube::CubeTransforms::B_prime,RubikCube::CubeTransforms::F_prime,RubikCube::CubeTransforms::D_prime,RubikCube::CubeTransforms::R_prime,RubikCube::CubeTransforms::L_prime,RubikCube::CubeTransforms::double_L,RubikCube::CubeTransforms::double_R,RubikCube::CubeTransforms::double_D,RubikCube::CubeTransforms::double_F,RubikCube::CubeTransforms::double_B},
  array{RubikCube::CubeTransforms::L,RubikCube::CubeTransforms::U,RubikCube::CubeTransforms::D,RubikCube::CubeTransforms::F,RubikCube::CubeTransforms::B,RubikCube::CubeTransforms::B_prime,RubikCube::CubeTransforms::F_prime,RubikCube::CubeTransforms::D_prime,RubikCube::CubeTransforms::U_prime,RubikCube::CubeTransforms::L_prime,RubikCube::CubeTransforms::double_L,RubikCube::CubeTransforms::double_U,RubikCube::CubeTransforms::double_D,RubikCube::CubeTransforms::double_F,RubikCube::CubeTransforms::double_B},
  array{RubikCube::CubeTransforms::R,RubikCube::CubeTransforms::U,RubikCube::CubeTransforms::D,RubikCube::CubeTransforms::F,RubikCube::CubeTransforms::B,RubikCube::CubeTransforms::B_prime,RubikCube::CubeTransforms::F_prime,RubikCube::CubeTransforms::D_prime,RubikCube::CubeTransforms::U_prime,RubikCube::CubeTransforms::R_prime,RubikCube::CubeTransforms::double_R,RubikCube::CubeTransforms::double_U,RubikCube::CubeTransforms::double_D,RubikCube::CubeTransforms::double_F,RubikCube::CubeTransforms::double_B},
  array{RubikCube::CubeTransforms::R,RubikCube::CubeTransforms::U,RubikCube::CubeTransforms::D,RubikCube::CubeTransforms::F,RubikCube::CubeTransforms::B,RubikCube::CubeTransforms::B_prime,RubikCube::CubeTransforms::F_prime,RubikCube::CubeTransforms::D_prime,RubikCube::CubeTransforms::U_prime,RubikCube::CubeTransforms::R_prime,RubikCube::CubeTransforms::double_R,RubikCube::CubeTransforms::double_U,RubikCube::CubeTransforms::double_D,RubikCube::CubeTransforms::double_F,RubikCube::CubeTransforms::double_B},
  array{RubikCube::CubeTransforms::L,RubikCube::CubeTransforms::U,RubikCube::CubeTransforms::D,RubikCube::CubeTransforms::F,RubikCube::CubeTransforms::B,RubikCube::CubeTransforms::B_prime,RubikCube::CubeTransforms::F_prime,RubikCube::CubeTransforms::D_prime,RubikCube::CubeTransforms::U_prime,RubikCube::CubeTransforms::L_prime,RubikCube::CubeTransforms::double_L,RubikCube::CubeTransforms::double_U,RubikCube::CubeTransforms::double_D,RubikCube::CubeTransforms::double_F,RubikCube::CubeTransforms::double_B},
  array{RubikCube::CubeTransforms::L,RubikCube::CubeTransforms::R,RubikCube::CubeTransforms::D,RubikCube::CubeTransforms::F,RubikCube::CubeTransforms::B,RubikCube::CubeTransforms::B_prime,RubikCube::CubeTransforms::F_prime,RubikCube::CubeTransforms::D_prime,RubikCube::CubeTransforms::R_prime,RubikCube::CubeTransforms::L_prime,RubikCube::CubeTransforms::double_L,RubikCube::CubeTransforms::double_R,RubikCube::CubeTransforms::double_D,RubikCube::CubeTransforms::double_F,RubikCube::CubeTransforms::double_B},
  array{RubikCube::CubeTransforms::L,RubikCube::CubeTransforms::R,RubikCube::CubeTransforms::U,RubikCube::CubeTransforms::F,RubikCube::CubeTransforms::B,RubikCube::CubeTransforms::B_prime,RubikCube::CubeTransforms::F_prime,RubikCube::CubeTransforms::U_prime,RubikCube::CubeTransforms::R_prime,RubikCube::CubeTransforms::L_prime,RubikCube::CubeTransforms::double_L,RubikCube::CubeTransforms::double_R,RubikCube::CubeTransforms::double_U,RubikCube::CubeTransforms::double_F,RubikCube::CubeTransforms::double_B},
  array{RubikCube::CubeTransforms::L,RubikCube::CubeTransforms::R,RubikCube::CubeTransforms::U,RubikCube::CubeTransforms::D,RubikCube::CubeTransforms::B,RubikCube::CubeTransforms::B_prime,RubikCube::CubeTransforms::D_prime,RubikCube::CubeTransforms::U_prime,RubikCube::CubeTransforms::R_prime,RubikCube::CubeTransforms::L_prime,RubikCube::CubeTransforms::double_L,RubikCube::CubeTransforms::double_R,RubikCube::CubeTransforms::double_U,RubikCube::CubeTransforms::double_D,RubikCube::CubeTransforms::double_B},
  array{RubikCube::CubeTransforms::L,RubikCube::CubeTransforms::R,RubikCube::CubeTransforms::U,RubikCube::CubeTransforms::D,RubikCube::CubeTransforms::F,RubikCube::CubeTransforms::F_prime,RubikCube::CubeTransforms::D_prime,RubikCube::CubeTransforms::U_prime,RubikCube::CubeTransforms::R_prime,RubikCube::CubeTransforms::L_prime,RubikCube::CubeTransforms::double_L,RubikCube::CubeTransforms::double_R,RubikCube::CubeTransforms::double_U,RubikCube::CubeTransforms::double_D,RubikCube::CubeTransforms::double_F}
};

void PrintOneSurface(const char *surface) {
	cout << kColorsDict[surface[0]] << ' ' << kColorsDict[surface[1]] << ' ' << kColorsDict[surface[2]] << '\n'
		<< kColorsDict[surface[3]] << " C " << kColorsDict[surface[4]] << '\n'
		<< kColorsDict[surface[5]] << ' ' << kColorsDict[surface[6]] << ' ' << kColorsDict[surface[7]] << '\n';
}

bool TrySolve(const RubikCube &origin_cube, RubikCube::CubeTransforms previous_step) {
  for (const auto trans : kAvailableTransforms[previous_step]) {
    RubikCube temp_cube = origin_cube;
    if (temp_cube.transform(trans)->isSolved()) {
      for (size_t i = 0; i < temp_cube.getStepCount(); ++i)
        cout << i << ": " << kCubeTransformsDict[temp_cube.getSteps()[i]] << '\t';
      cout << '\n';
      return true;
    }
    if (temp_cube.getStepCount() < kMaxSteps)
      TrySolve(temp_cube, trans);
  }
  return false;
}

int main(void) {
	// __m128i test_val = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	// __m128i test_val3 = test_val;
	// char test_val2[16] = { 0 };
	// memcpy(test_val2, &test_val, sizeof(test_val));

	//return 0;

	RubikCube cube(kInitialCube);
	cube.transform(RubikCube::CubeTransforms::R);
	cube.transform(RubikCube::CubeTransforms::U);
	cube.transform(RubikCube::CubeTransforms::R);
	cube.transform(RubikCube::CubeTransforms::U);
	cube.transform(RubikCube::CubeTransforms::R);
	cube.transform(RubikCube::CubeTransforms::U_prime);
	cube.transform(RubikCube::CubeTransforms::R_prime);
	// cube.transform(RubikCube::CubeTransforms::U_prime);
	// cube.transform(RubikCube::CubeTransforms::R_prime);
	// cube.transform(RubikCube::CubeTransforms::U_prime);
	cube.resetStep();
	for (const auto trans : kCubeTransforms) {
		RubikCube temp_cube = cube;
		if (temp_cube.transform(trans)->isSolved()) {
			cout << kCubeTransformsDict[trans] << ' ';
			return 0;
		}
		if (TrySolve(temp_cube, trans)) {
			return 0;
		}
	}



	return 0;
}
