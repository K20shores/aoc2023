#include <benchmark/benchmark.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

struct Data {
  std::vector<int> times;
  std::vector<int> distance;
};

int64_t quad_solution(int64_t T, int64_t D) {
  double disc = std::sqrt(T * T - 4 * D);
  int64_t x = std::ceil((T - disc) / 2);
  int64_t y = std::floor((T + disc) / 2);
  return y - x + 1 - 2 * (T % 2 == 0);
}

int64_t part1(const Data &data) {
  long result = 1;
  for (size_t i = 0; i < data.times.size(); ++i) {
    result *= quad_solution(data.times[i], data.distance[i]);
  }
  return result;
}

int64_t part2(const Data &data) {
  std::string T = "";
  std::string D = "";
  for (size_t i = 0; i < data.times.size(); ++i) {
    T += std::to_string(data.times[i]);
    D += std::to_string(data.distance[i]);
  }

  return quad_solution(std::stoll(T), std::stoll(D));
}

std::vector<int> parse_numbers(const std::string &line) {
  std::vector<int> numbers;
  size_t num_start = -1;
  for (size_t i = 0; i <= line.size(); ++i) {
    if (std::isdigit(line[i])) {
      if (num_start == -1) {
        num_start = i;
      }
    } else if (num_start != -1) {
      numbers.push_back(std::stol(line.substr(num_start, i - num_start)));
      num_start = -1;
    }
  }
  return numbers;
}

Data parse() {
  std::ifstream file(std::filesystem::path("inputs/day6.txt"));
  if (!file) {
    std::cout << "Current Working Directory: " << std::filesystem::current_path() << std::endl;
    throw std::runtime_error("Bad file read");
  }
  std::string line;
  Data data;

  while (std::getline(file, line)) {
    if (data.times.size() == 0) {
      data.times = parse_numbers(line);
    } else {
      data.distance = parse_numbers(line);
    }
  }

  return data;
}

class BenchmarkFixture : public benchmark::Fixture {
public:
  static Data data;
};

Data BenchmarkFixture::data = parse();

BENCHMARK_DEFINE_F(BenchmarkFixture, Part1Benchmark)
(benchmark::State &state) {
  for (auto _ : state) {
    int s = part1(data);
    benchmark::DoNotOptimize(s);
  }
}

BENCHMARK_DEFINE_F(BenchmarkFixture, Part2Benchmark)
(benchmark::State &state) {
  for (auto _ : state) {
    int s = part2(data);
    benchmark::DoNotOptimize(s);
  }
}

BENCHMARK_REGISTER_F(BenchmarkFixture, Part1Benchmark);
BENCHMARK_REGISTER_F(BenchmarkFixture, Part2Benchmark);

int main(int argc, char **argv) {
  Data data = parse();

  std::cout << "Part 1: " << part1(data) << std::endl;
  std::cout << "Part 2: " << part2(data) << std::endl;

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
}