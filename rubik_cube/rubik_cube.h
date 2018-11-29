
#ifndef _RUBIK_CUBE_H_
#define _RUBIK_CUBE_H_

class RubikCube {
 public:
  enum Colors { Red, Orange, Green, Blue, Yellow, White };

  RubikCube() = delete;
  RubikCube(const RubikCube &y) : RubikCube(y.getCube()) {}
  RubikCube(const char *);
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
    L_prime,
    R_prime,
    U_prime,
    D_prime,
    F_prime,
    B_prime
  };

  bool isSolved() const;

  void transform(CubeTransforms t);

  const char *getCube() const { return overall_; };

  bool operator==(const RubikCube &) const;
  RubikCube &operator=(const RubikCube &);

 private:
  char overall_[48];
  char *const up_;
  char *const down_;
  char *const front_;
  char *const back_;
  char *const left_;
  char *const right_;
};

#endif  // !_RUBIK_CUBE_H_
