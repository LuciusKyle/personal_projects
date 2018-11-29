
#include "rubik_cube.h"

#include <immintrin.h>
#include <tbb/concurrent_hash_map.h>
#include <tbb/parallel_for.h>
#include <array>
#include <iostream>

using std::cout;
using std::endl;

constexpr size_t kCubeSize = 48;

const RubikCube initial_cube(
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

constexpr std::array kColorsDict{'R', 'O', 'G', 'B', 'Y', 'W'};
void print_one_surface(const char *surface){
  cout << kColorsDict[surface[0]] << ' ' << kColorsDict[surface[1]] << ' ' << kColorsDict[surface[2]] << '\n'
       << kColorsDict[surface[3]] << " C " << kColorsDict[surface[4]] << '\n'
       << kColorsDict[surface[5]] << ' ' << kColorsDict[surface[6]] << ' ' << kColorsDict[surface[7]] << '\n';
}

int main(void) {
  RubikCube cube(initial_cube);
  cube.transform(RubikCube::CubeTransforms::U_prime);
  char this_cube[kCubeSize] = {0};
  memcpy(this_cube, cube.getCube(), kCubeSize);
  for (size_t i = 0; i < 6; ++i) {
    print_one_surface(this_cube + i * 8);
    cout << "\n" << endl;
  }

  for(auto var:RubikCube::CubeTransforms){
    
  }

  return 0;
}

