
#define _CRT_SECURE_NO_WARNINGS


#include "rubik_cube.h"

#include <immintrin.h>
#include<tbb/tbb.h>
#include<unordered_set>
#include <array>
#include<chrono>
#include<boost/program_options.hpp>
#include <iostream>
#include <filesystem>
#include<string_view>

using std::array;
using std::cout;
using std::endl;
using std::string;

constexpr unsigned int kMaxSteps = 8;


int IntergerPower(int base, int exponent) {
	if (exponent == 0) return 1;
	if (exponent == 1) return base;
	return base * IntergerPower(base, exponent - 1);
}


void PrintOneSurface(const char *surface) {
	cout << kColorsDict[surface[0]] << ' ' << kColorsDict[surface[1]] << ' ' << kColorsDict[surface[2]] << '\n'
		<< kColorsDict[surface[3]] << " C " << kColorsDict[surface[4]] << '\n'
		<< kColorsDict[surface[5]] << ' ' << kColorsDict[surface[6]] << ' ' << kColorsDict[surface[7]] << '\n';
}

struct MyHashCompare {
	size_t hash(const RubikCube& x) {
		size_t h = 0;
		const char *s = x.getCube();
		for (size_t i = 0; i < 48; ++i)
			h = (h * 17) ^ (s[i]);
		return h;
	}
	//! True if strings are equal
	bool equal(const RubikCube& x, const RubikCube& y) {
		return x == y;
	}
};

bool TrySolve(const RubikCube &origin_cube, const RubikCube::CubeTransforms previous_step) {
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


//tbb::tbb_hash<RubikCube> cube_hash;


int main01(const int argc, const char *argv[]) {
	// __m128i test_val = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	// __m128i test_val3 = test_val;
	// char test_val2[16] = { 0 };
	// memcpy(test_val2, &test_val, sizeof(test_val));
	//return 0;

	namespace po = boost::program_options;
	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("host", po::value<string>(), "MySQL host, IP or domain")
		("port", po::value<unsigned short>(), "default port is 33060")
		("user", po::value<string>(), "MySQL user")
		("password", po::value<string>(), "password")
		("O", po::value<string>(), "out put file path (not file name)");

	po::variables_map vm;
	try {
		auto rtn = po::parse_command_line(argc, argv, desc);
		po::store(rtn, vm);
	}
	catch (std::exception &ex) {
		cout << ex.what() << endl;
		return -1;
	}
	po::notify(vm);

	if (vm.count("help")) {
		cout << desc << "\n";
		return 1;
	}

	const string host = vm.count("host") > 0 ? vm["host"].as<string>() : "47.107.180.107";
	const unsigned short port = vm.count("port") > 0 ? vm["port"].as<unsigned short>() : 33060;
	const string user = vm.count("user") > 0 ? vm["user"].as<string>() : "Kyle";
	const string password = vm.count("password") > 0 ? vm["password"].as<string>() : "password";
	const string out_file_path = vm.count("O") > 0 ? vm["O"].as<string>() : "./out.file";


	RubikCube cube(kInitialCube);
	cube.transform(RubikCube::CubeTransforms::R);
	cube.transform(RubikCube::CubeTransforms::U);
	cube.transform(RubikCube::CubeTransforms::R);
	cube.transform(RubikCube::CubeTransforms::U);
	cube.transform(RubikCube::CubeTransforms::R);
	cube.transform(RubikCube::CubeTransforms::U_prime);
	cube.transform(RubikCube::CubeTransforms::R_prime);
	cube.transform(RubikCube::CubeTransforms::U_prime);
	//cube.transform(RubikCube::CubeTransforms::R_prime);
	//cube.transform(RubikCube::CubeTransforms::U_prime);
	cube.resetStep();

	auto start = std::chrono::system_clock::now();
	tbb::parallel_for(tbb::blocked_range<int>(0, static_cast<int>(kCubeTransforms.size())), [cube](const tbb::blocked_range<int> &r) {
		for (int trans = r.begin(); trans != r.end(); ++trans) {
			RubikCube temp_cube = cube;
			if (temp_cube.transform(static_cast<RubikCube::CubeTransforms>(trans))->isSolved()) {
				cout << kCubeTransformsDict[trans] << ' ';
				return;
			}
			TrySolve(temp_cube, static_cast<RubikCube::CubeTransforms>(trans));
		}
	});

	cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() << endl;

	return 0;
}

int main02(const int argc, const char *argv[]) {
	boost::program_options::variables_map vm;
	{
		namespace po = boost::program_options;
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("level", po::value<unsigned int>(), "level(s), no more than 11")
			("O", po::value<string>(), "out put file path (not file name)");

		try {
			auto rtn = po::parse_command_line(argc, argv, desc);
			po::store(rtn, vm);
		}
		catch (std::exception &ex) {
			cout << ex.what() << endl;
			return -1;
		}
		po::notify(vm);

		if (vm.count("help")) {
			cout << desc << "\n";
			return 1;
		}
	}
	const unsigned int levels = vm.count("level") > 0 ? vm["level"].as<unsigned int>() : 1;
	const string out_file_path = vm.count("O") > 0 ? vm["O"].as<string>() : "cube_";

	size_t cur_pos = 0;
	char *file_buffer = new char[18 * kCubeMemSize + IntergerPower(15, levels) * kCubeMemSize];
	for (const auto trans : kCubeTransforms) {
		RubikCube cube(kInitialCube);
		cube.transform(trans);
		memcpy(file_buffer + cur_pos, &cube, kCubeMemSize);
		cur_pos += kCubeMemSize;
	}
	FILE *fp = fopen((out_file_path + "_level_1").c_str(), "wb");
	fwrite(file_buffer, 18 * kCubeMemSize, 1, fp);
	fclose(fp);

	const auto start = std::chrono::system_clock::now();

	for (unsigned int i = 1; i < levels; ++i) {

		fp = fopen((out_file_path + "_level_" + std::to_string(i)).c_str(), "rb");
		fseek(fp, 0, SEEK_END);
		const long file_size = ftell(fp);
		fseek(fp, 0, SEEK_SET);


		fread(file_buffer, file_size, 1, fp);
		fclose(fp);
		const size_t base_cubes_count = file_size / kCubeMemSize;
		char *next_level_buffer = new char[base_cubes_count * 15 * kCubeMemSize];

		if (0x80 < base_cubes_count) {
			tbb::parallel_for(size_t(0), base_cubes_count, [file_buffer, next_level_buffer](size_t ii) {
				const RubikCube *temp_ptr = reinterpret_cast<const RubikCube *>(file_buffer + ii * kCubeMemSize);
				const auto &arr = kAvailableTransforms[temp_ptr->getSteps()[temp_ptr->getStepCount() - 1]];
				for (size_t iii = 0; iii < arr.size(); ++iii) {
					RubikCube temp_cube(*temp_ptr);
					temp_cube.transform(arr[iii]);
					memcpy(next_level_buffer + (ii * arr.size() + iii) * kCubeMemSize, &temp_cube, kCubeMemSize);
				}
				});
		}
		else {
			for (size_t ii = 0; ii < base_cubes_count; ++ii) {
				const RubikCube *temp_ptr = reinterpret_cast<const RubikCube *>(file_buffer + ii * kCubeMemSize);
				const auto &arr = kAvailableTransforms[temp_ptr->getSteps()[temp_ptr->getStepCount() - 1]];
				for (size_t iii = 0; iii < arr.size(); ++iii) {
					RubikCube temp_cube(*temp_ptr);
					temp_cube.transform(arr[iii]);
					memcpy(next_level_buffer + (ii * arr.size() + iii) * kCubeMemSize, &temp_cube, kCubeMemSize);
				}
			}
		}
		fp = fopen((out_file_path + "_level_" + std::to_string(i + 1)).c_str(), "wb");
		fwrite(next_level_buffer, base_cubes_count * 15 * kCubeMemSize, 1, fp);
		fclose(fp);
		delete[] next_level_buffer;
	}
	delete[] file_buffer;

	cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() << " ms" << endl;

	return 0;
}

void test_ConstructCompressCube() {
	const size_t kTestSampleCount = 100'000'000;
	RubikCube test_sample = kInitialCube;
	char *temp_ptr = new char[sizeof(CompressedCube) * kTestSampleCount];
	auto start = std::chrono::system_clock::now();
	for (size_t i = 0; i < kTestSampleCount; ++i) {
		new (temp_ptr + i * sizeof(CompressedCube)) CompressedCube(test_sample);
	}
	cout << "construct " << kTestSampleCount << "CompressedCube cost: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() << " ms" << endl;
	delete[] temp_ptr;
}

bool BinSearchCubeExist(const CompressedCube *sorted, const size_t cube_count, const CompressedCube &target) {
	size_t lower = 0;
	size_t upper = cube_count - 1;

	while (1 < upper - lower) {
		const size_t mid = (upper + lower) / 2;
		if (sorted[mid] < target)
			lower = mid;
		else if (target < sorted[mid])
			upper = mid;
		else
			return true;
	}	
	return sorted[lower] == target || sorted[upper] == target;
}

void createFileForFirst8Steps() {
	{
		FILE *fp = fopen("cube_level_0", "wb");
		fwrite(&kInitialCube, sizeof(kInitialCube), 1, fp);
		fclose(fp);
	}

	const string cube_collection_file_name("cube_collection");

	for (size_t i = 1; i < 7; ++i) {
		const string file_name = "cube_level_" + std::to_string(i);
		const string pre_file_name = "cube_level_" + std::to_string(i - 1);
		const size_t pre_file_size = std::filesystem::file_size(pre_file_name);
		assert(pre_file_size != 0 && (pre_file_size % sizeof(RubikCube)) == 0);
		char *pre_file_content = new char[pre_file_size];
		FILE *fp = fopen(pre_file_name.c_str(), "rb");
		fread(pre_file_content, pre_file_size, 1, fp);
		fclose(fp);
		char *next_level_buffer = new char[pre_file_size * 15];
		for (size_t ii = 0; ii < pre_file_size / sizeof(RubikCube); ++ii) {
			const RubikCube *temp_ptr = reinterpret_cast<const RubikCube *>(pre_file_content + ii * sizeof(RubikCube));
			const auto &arr = kAvailableTransforms[temp_ptr->getSteps()[temp_ptr->getStepCount() - 1]];
			for (size_t iii = 0; iii < arr.size(); ++iii) {
				RubikCube temp_cube(*temp_ptr);
				temp_cube.transform(arr[iii]);
				memcpy(next_level_buffer + (ii * arr.size() + iii) * sizeof(RubikCube), &temp_cube, sizeof(RubikCube));
			}
		}
		size_t cur_pos = 0;
		fp = fopen(cube_collection_file_name.c_str(), "rb");
		if (size_t the_file_size = std::filesystem::file_size(pre_file_name); pre_file_size < the_file_size) {
			delete[] pre_file_content;
			pre_file_content = new char[the_file_size];
		}
		for (size_t ii = 0; ii < pre_file_size / sizeof(RubikCube) * 15; ++ii) {
			
		}
	}
}

int main(void) {

	test_ConstructCompressCube();

	return 0;
}