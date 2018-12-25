#ifndef _RUBIK_CUBE_H_
#define _RUBIK_CUBE_H_

#include<array>
#include<cstddef> // size_t

class RubikCube {
public:
	enum Colors { Red, Orange, Green, Blue, Yellow, White };

	RubikCube() = delete;
	RubikCube(const RubikCube &y) : RubikCube(y.getCube(), y.getSteps(), y.getStepCount()) {}
	RubikCube(const char *overall, const char *steps, const size_t step_count);
	RubikCube(Colors up_0, Colors up_1, Colors up_2, Colors up_3, Colors up_4,
		Colors up_5, Colors up_6, Colors up_7, Colors down_0, Colors down_1,
		Colors down_2, Colors down_3, Colors down_4, Colors down_5,
		Colors down_6, Colors down_7, Colors front_0, Colors front_1,
		Colors front_2, Colors front_3, Colors front_4, Colors front_5,
		Colors front_6, Colors front_7, Colors back_0, Colors back_1,
		Colors back_2, Colors back_3, Colors back_4, Colors back_5,
		Colors back_6, Colors back_7, Colors left_0, Colors left_1,
		Colors left_2, Colors left_3, Colors left_4, Colors left_5,
		Colors left_6, Colors left_7, Colors right_0, Colors right_1,
		Colors right_2, Colors right_3, Colors right_4, Colors right_5,
		Colors right_6, Colors right_7);
	enum CubeTransforms {
		L,
		R,
		U,
		D,
		F,
		B,
		B_prime,
		F_prime,
		D_prime,
		U_prime,
		R_prime,
		L_prime,
		double_L,
		double_R,
		double_U,
		double_D,
		double_F,
		double_B
	};

	bool isSolved() const;

	const RubikCube* transform(CubeTransforms t);

	const char *getCube() const { return overall_; };

	bool operator==(const RubikCube &) const;
	RubikCube &operator=(const RubikCube &);

	const char *getSteps() const { return steps_; }
	size_t getStepCount() const { return step_count_; }
	void resetStep();
private:
	char overall_[48];
	char steps_[15];
	unsigned char step_count_;
};

class CompressedCube {
public:
	explicit CompressedCube(const char *origin_cube);
	CompressedCube(const RubikCube &origin_cube);
	CompressedCube(const CompressedCube &);
	CompressedCube() = delete;

	bool operator==(const CompressedCube &) const;
	bool operator<(const CompressedCube &) const;

	const unsigned char compressed_cube_[18];
};

constexpr size_t kCubeSize = 48;
constexpr size_t kCubeMemSize = sizeof(RubikCube);

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

constexpr std::array kColorsDict{ 'R', 'O', 'G', 'B', 'Y', 'W' };
constexpr std::array kCubeTransforms{
	RubikCube::CubeTransforms::L,        RubikCube::CubeTransforms::R,
	RubikCube::CubeTransforms::U,        RubikCube::CubeTransforms::D,
	RubikCube::CubeTransforms::F,        RubikCube::CubeTransforms::B,
	RubikCube::CubeTransforms::L_prime,  RubikCube::CubeTransforms::R_prime,
	RubikCube::CubeTransforms::U_prime,  RubikCube::CubeTransforms::D_prime,
	RubikCube::CubeTransforms::F_prime,  RubikCube::CubeTransforms::B_prime,
	RubikCube::CubeTransforms::double_L, RubikCube::CubeTransforms::double_R,
	RubikCube::CubeTransforms::double_U, RubikCube::CubeTransforms::double_D,
	RubikCube::CubeTransforms::double_F, RubikCube::CubeTransforms::double_B };
constexpr std::array<std::string_view, 18> kCubeTransformsDict{ "L", "R", "U", "D", "F", "B", "B'", "F'", "D'", "U'", "R'", "L'","L2", "R2", "U2", "D2", "F2", "B2" };

constexpr std::array kAvailableTransforms{
  std::array{ RubikCube::CubeTransforms::R, RubikCube::CubeTransforms::U, RubikCube::CubeTransforms::D, RubikCube::CubeTransforms::F, RubikCube::CubeTransforms::B, RubikCube::CubeTransforms::B_prime, RubikCube::CubeTransforms::F_prime, RubikCube::CubeTransforms::D_prime, RubikCube::CubeTransforms::U_prime, RubikCube::CubeTransforms::R_prime, RubikCube::CubeTransforms::double_R, RubikCube::CubeTransforms::double_U, RubikCube::CubeTransforms::double_D, RubikCube::CubeTransforms::double_F, RubikCube::CubeTransforms::double_B },
  std::array{ RubikCube::CubeTransforms::L, RubikCube::CubeTransforms::U, RubikCube::CubeTransforms::D, RubikCube::CubeTransforms::F, RubikCube::CubeTransforms::B, RubikCube::CubeTransforms::B_prime, RubikCube::CubeTransforms::F_prime, RubikCube::CubeTransforms::D_prime, RubikCube::CubeTransforms::U_prime, RubikCube::CubeTransforms::L_prime, RubikCube::CubeTransforms::double_L, RubikCube::CubeTransforms::double_U, RubikCube::CubeTransforms::double_D, RubikCube::CubeTransforms::double_F, RubikCube::CubeTransforms::double_B },
  std::array{ RubikCube::CubeTransforms::L, RubikCube::CubeTransforms::R, RubikCube::CubeTransforms::D, RubikCube::CubeTransforms::F, RubikCube::CubeTransforms::B, RubikCube::CubeTransforms::B_prime, RubikCube::CubeTransforms::F_prime, RubikCube::CubeTransforms::D_prime, RubikCube::CubeTransforms::R_prime, RubikCube::CubeTransforms::L_prime, RubikCube::CubeTransforms::double_L, RubikCube::CubeTransforms::double_R, RubikCube::CubeTransforms::double_D, RubikCube::CubeTransforms::double_F, RubikCube::CubeTransforms::double_B },
  std::array{ RubikCube::CubeTransforms::L, RubikCube::CubeTransforms::R, RubikCube::CubeTransforms::U, RubikCube::CubeTransforms::F, RubikCube::CubeTransforms::B, RubikCube::CubeTransforms::B_prime, RubikCube::CubeTransforms::F_prime, RubikCube::CubeTransforms::U_prime, RubikCube::CubeTransforms::R_prime, RubikCube::CubeTransforms::L_prime, RubikCube::CubeTransforms::double_L, RubikCube::CubeTransforms::double_R, RubikCube::CubeTransforms::double_U, RubikCube::CubeTransforms::double_F, RubikCube::CubeTransforms::double_B },
  std::array{ RubikCube::CubeTransforms::L, RubikCube::CubeTransforms::R, RubikCube::CubeTransforms::U, RubikCube::CubeTransforms::D, RubikCube::CubeTransforms::B, RubikCube::CubeTransforms::B_prime, RubikCube::CubeTransforms::D_prime, RubikCube::CubeTransforms::U_prime, RubikCube::CubeTransforms::R_prime, RubikCube::CubeTransforms::L_prime, RubikCube::CubeTransforms::double_L, RubikCube::CubeTransforms::double_R, RubikCube::CubeTransforms::double_U, RubikCube::CubeTransforms::double_D, RubikCube::CubeTransforms::double_B },
  std::array{ RubikCube::CubeTransforms::L, RubikCube::CubeTransforms::R, RubikCube::CubeTransforms::U, RubikCube::CubeTransforms::D, RubikCube::CubeTransforms::F, RubikCube::CubeTransforms::F_prime, RubikCube::CubeTransforms::D_prime, RubikCube::CubeTransforms::U_prime, RubikCube::CubeTransforms::R_prime, RubikCube::CubeTransforms::L_prime, RubikCube::CubeTransforms::double_L, RubikCube::CubeTransforms::double_R, RubikCube::CubeTransforms::double_U, RubikCube::CubeTransforms::double_D, RubikCube::CubeTransforms::double_F },
  std::array{ RubikCube::CubeTransforms::L, RubikCube::CubeTransforms::R, RubikCube::CubeTransforms::U, RubikCube::CubeTransforms::D, RubikCube::CubeTransforms::F, RubikCube::CubeTransforms::F_prime, RubikCube::CubeTransforms::D_prime, RubikCube::CubeTransforms::U_prime, RubikCube::CubeTransforms::R_prime, RubikCube::CubeTransforms::L_prime, RubikCube::CubeTransforms::double_L, RubikCube::CubeTransforms::double_R, RubikCube::CubeTransforms::double_U, RubikCube::CubeTransforms::double_D, RubikCube::CubeTransforms::double_F },
  std::array{ RubikCube::CubeTransforms::L, RubikCube::CubeTransforms::R, RubikCube::CubeTransforms::U, RubikCube::CubeTransforms::D, RubikCube::CubeTransforms::B, RubikCube::CubeTransforms::B_prime, RubikCube::CubeTransforms::D_prime, RubikCube::CubeTransforms::U_prime, RubikCube::CubeTransforms::R_prime, RubikCube::CubeTransforms::L_prime, RubikCube::CubeTransforms::double_L, RubikCube::CubeTransforms::double_R, RubikCube::CubeTransforms::double_U, RubikCube::CubeTransforms::double_D, RubikCube::CubeTransforms::double_B },
  std::array{ RubikCube::CubeTransforms::L, RubikCube::CubeTransforms::R, RubikCube::CubeTransforms::U, RubikCube::CubeTransforms::F, RubikCube::CubeTransforms::B, RubikCube::CubeTransforms::B_prime, RubikCube::CubeTransforms::F_prime, RubikCube::CubeTransforms::U_prime, RubikCube::CubeTransforms::R_prime, RubikCube::CubeTransforms::L_prime, RubikCube::CubeTransforms::double_L, RubikCube::CubeTransforms::double_R, RubikCube::CubeTransforms::double_U, RubikCube::CubeTransforms::double_F, RubikCube::CubeTransforms::double_B },
  std::array{ RubikCube::CubeTransforms::L, RubikCube::CubeTransforms::R, RubikCube::CubeTransforms::D, RubikCube::CubeTransforms::F, RubikCube::CubeTransforms::B, RubikCube::CubeTransforms::B_prime, RubikCube::CubeTransforms::F_prime, RubikCube::CubeTransforms::D_prime, RubikCube::CubeTransforms::R_prime, RubikCube::CubeTransforms::L_prime, RubikCube::CubeTransforms::double_L, RubikCube::CubeTransforms::double_R, RubikCube::CubeTransforms::double_D, RubikCube::CubeTransforms::double_F, RubikCube::CubeTransforms::double_B },
  std::array{ RubikCube::CubeTransforms::L, RubikCube::CubeTransforms::U, RubikCube::CubeTransforms::D, RubikCube::CubeTransforms::F, RubikCube::CubeTransforms::B, RubikCube::CubeTransforms::B_prime, RubikCube::CubeTransforms::F_prime, RubikCube::CubeTransforms::D_prime, RubikCube::CubeTransforms::U_prime, RubikCube::CubeTransforms::L_prime, RubikCube::CubeTransforms::double_L, RubikCube::CubeTransforms::double_U, RubikCube::CubeTransforms::double_D, RubikCube::CubeTransforms::double_F, RubikCube::CubeTransforms::double_B },
  std::array{ RubikCube::CubeTransforms::R, RubikCube::CubeTransforms::U, RubikCube::CubeTransforms::D, RubikCube::CubeTransforms::F, RubikCube::CubeTransforms::B, RubikCube::CubeTransforms::B_prime, RubikCube::CubeTransforms::F_prime, RubikCube::CubeTransforms::D_prime, RubikCube::CubeTransforms::U_prime, RubikCube::CubeTransforms::R_prime, RubikCube::CubeTransforms::double_R, RubikCube::CubeTransforms::double_U, RubikCube::CubeTransforms::double_D, RubikCube::CubeTransforms::double_F, RubikCube::CubeTransforms::double_B },
  std::array{ RubikCube::CubeTransforms::R, RubikCube::CubeTransforms::U, RubikCube::CubeTransforms::D, RubikCube::CubeTransforms::F, RubikCube::CubeTransforms::B, RubikCube::CubeTransforms::B_prime, RubikCube::CubeTransforms::F_prime, RubikCube::CubeTransforms::D_prime, RubikCube::CubeTransforms::U_prime, RubikCube::CubeTransforms::R_prime, RubikCube::CubeTransforms::double_R, RubikCube::CubeTransforms::double_U, RubikCube::CubeTransforms::double_D, RubikCube::CubeTransforms::double_F, RubikCube::CubeTransforms::double_B },
  std::array{ RubikCube::CubeTransforms::L, RubikCube::CubeTransforms::U, RubikCube::CubeTransforms::D, RubikCube::CubeTransforms::F, RubikCube::CubeTransforms::B, RubikCube::CubeTransforms::B_prime, RubikCube::CubeTransforms::F_prime, RubikCube::CubeTransforms::D_prime, RubikCube::CubeTransforms::U_prime, RubikCube::CubeTransforms::L_prime, RubikCube::CubeTransforms::double_L, RubikCube::CubeTransforms::double_U, RubikCube::CubeTransforms::double_D, RubikCube::CubeTransforms::double_F, RubikCube::CubeTransforms::double_B },
  std::array{ RubikCube::CubeTransforms::L, RubikCube::CubeTransforms::R, RubikCube::CubeTransforms::D, RubikCube::CubeTransforms::F, RubikCube::CubeTransforms::B, RubikCube::CubeTransforms::B_prime, RubikCube::CubeTransforms::F_prime, RubikCube::CubeTransforms::D_prime, RubikCube::CubeTransforms::R_prime, RubikCube::CubeTransforms::L_prime, RubikCube::CubeTransforms::double_L, RubikCube::CubeTransforms::double_R, RubikCube::CubeTransforms::double_D, RubikCube::CubeTransforms::double_F, RubikCube::CubeTransforms::double_B },
  std::array{ RubikCube::CubeTransforms::L, RubikCube::CubeTransforms::R, RubikCube::CubeTransforms::U, RubikCube::CubeTransforms::F, RubikCube::CubeTransforms::B, RubikCube::CubeTransforms::B_prime, RubikCube::CubeTransforms::F_prime, RubikCube::CubeTransforms::U_prime, RubikCube::CubeTransforms::R_prime, RubikCube::CubeTransforms::L_prime, RubikCube::CubeTransforms::double_L, RubikCube::CubeTransforms::double_R, RubikCube::CubeTransforms::double_U, RubikCube::CubeTransforms::double_F, RubikCube::CubeTransforms::double_B },
  std::array{ RubikCube::CubeTransforms::L, RubikCube::CubeTransforms::R, RubikCube::CubeTransforms::U, RubikCube::CubeTransforms::D, RubikCube::CubeTransforms::B, RubikCube::CubeTransforms::B_prime, RubikCube::CubeTransforms::D_prime, RubikCube::CubeTransforms::U_prime, RubikCube::CubeTransforms::R_prime, RubikCube::CubeTransforms::L_prime, RubikCube::CubeTransforms::double_L, RubikCube::CubeTransforms::double_R, RubikCube::CubeTransforms::double_U, RubikCube::CubeTransforms::double_D, RubikCube::CubeTransforms::double_B },
  std::array{ RubikCube::CubeTransforms::L, RubikCube::CubeTransforms::R, RubikCube::CubeTransforms::U, RubikCube::CubeTransforms::D, RubikCube::CubeTransforms::F, RubikCube::CubeTransforms::F_prime, RubikCube::CubeTransforms::D_prime, RubikCube::CubeTransforms::U_prime, RubikCube::CubeTransforms::R_prime, RubikCube::CubeTransforms::L_prime, RubikCube::CubeTransforms::double_L, RubikCube::CubeTransforms::double_R, RubikCube::CubeTransforms::double_U, RubikCube::CubeTransforms::double_D, RubikCube::CubeTransforms::double_F }
};

#endif // !_RUBIK_CUBE_H_