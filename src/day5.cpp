#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <numeric>
#include <benchmark/benchmark.h>
#include <queue>

struct Mapping
{
  long src;
  long dest;
  long length;
};

struct Range
{
  // inclusive
  long start;
  // exclusive
  long end;
  long size;
};

struct Data
{
  std::vector<long> seeds;
  std::vector<std::vector<Mapping>> mappings;
};

long map_range(const Range &first_range, const Data &data)
{
  auto s = first_range.size;
  std::cout << "\n-----\n";
  std::vector<Range> ranges = {first_range};
  for (const auto &mappings : data.mappings)
  {
    for (auto &range : ranges)
    {
      std::cout << "start " << range.start << " end " << range.end << " length " << range.size << std::endl;
    }
    std::cout << std::endl;
    std::vector<Range> new_ranges;
    std::queue<Range> working_set;
    for(auto& range: ranges) working_set.push(range);
    while(working_set.size() > 0) 
    {
      auto range = working_set.front();
      working_set.pop();
      bool mapped = false;
      for (const auto &mapping : mappings)
      {
        // mapping: |             [      )
        // range:   |     [          )
        if (range.start <= mapping.src && mapping.src < range.end)
        {
          mapped = true;
          if (range.start < mapping.src && mapping.src + mapping.length < range.end) {
            // the mapping pulls out a chunk from the middle of the range
            // the middle chunk
            Range r = {.start = mapping.dest, .end = mapping.dest + mapping.length, .size = mapping.length};
            new_ranges.push_back(r);
            // the part past the end of the chunk that may still be mapped by another mapping
            Range r2 = {.start = mapping.src + mapping.length, .end = range.end, .size = range.end - (mapping.src + mapping.length)};
            working_set.push(r2);

            // the part not mapped
            range.end = mapping.src;
            range.size = mapping.src - range.start;
            new_ranges.push_back(range);
          }
          else if (range.start < mapping.src && mapping.src + mapping.length > range.end) {
            // the mapping goes past the end of the range
            // the part that overlaps
            auto size = range.end - mapping.src;
            Range r = {.start = mapping.dest, .end = mapping.dest + size, .size = size};
            // the part not mapped
            range.end = mapping.src;
            range.size = mapping.src - range.start;
            new_ranges.push_back(r);
            new_ranges.push_back(range);
          }
          else {
            // the mapping and the range exactly overlap
            auto size = std::min(mapping.length, range.size);
            range.start = mapping.dest;
            range.end = mapping.dest + size;
            range.size = size;
            new_ranges.push_back(range);
          }
          break;
        }
        // range:   |             [      )
        // mapping: |     [          )
        else if (mapping.src <= range.start && range.start < (mapping.src + mapping.length))
        {
          mapped = true;
          // map the overlap
          auto start = mapping.dest + (range.start - mapping.src);
          auto size = std::min(range.size, mapping.src + mapping.length - range.start);
          auto end = start + size;

          if (size < range.size)
          {
            // there was a partial overlap
            Range r = {.start = start, .end = end, .size = size};

            // update the range
            range.start += size;
            range.size -= r.size;

            new_ranges.push_back(r);
            new_ranges.push_back(range);
          }
          else
          {
            // the range was entirely within the mapping
            range.start = start;
            range.end = end;
            range.size = size;
            new_ranges.push_back(range);
          }
          break;
        }
      }
      if (!mapped) new_ranges.push_back(range);
    }
    ranges = new_ranges;
    auto p = 0;
    for (const auto &range : ranges)
    {
      p += range.size;
    }
    if (p != s)
      throw std::runtime_error("sizes not equal");
  }
  for (auto &range : ranges)
  {
    std::cout << "start " << range.start << " end " << range.end << " length " << range.size << std::endl;
  }
  std::cout << std::endl;
  long min = std::numeric_limits<long>::max();
  for (const auto &range : ranges)
  {
    min = std::min(min, range.start);
  }
  std::cout << min << std::endl;
  std::cout << "\n=====\n";
  return min;
}

long part1(const Data &data)
{
  long min = std::numeric_limits<long>::max();
  for (size_t i = 0; i < data.seeds.size(); i += 1)
  {
    long start = data.seeds[i];
    min = std::min(min, map_range({.start = start, .end = start + 1, .size = 1}, data));
  }
  return min;
}

int part2(const Data &data)
{
  long min = std::numeric_limits<long>::max();
  for (size_t i = 0; i < data.seeds.size(); i += 2)
  {
    long start = data.seeds[i];
    long n = data.seeds[i + 1];
    min = std::min(min, map_range({.start = start, .end = start + n, .size = n}, data));
  }
  return min;
}

std::vector<long> parse_numbers(const std::string &line)
{
  std::vector<long> numbers;
  size_t num_start = -1;
  for (size_t i = 0; i <= line.size(); ++i)
  {
    if (std::isdigit(line[i]))
    {
      if (num_start == -1)
      {
        num_start = i;
      }
    }
    else if (num_start != -1)
    {
      numbers.push_back(std::stol(line.substr(num_start, i - num_start)));
      num_start = -1;
    }
  }
  return numbers;
}

bool compare_mappings(const Mapping &a, const Mapping &b)
{
  return a.src < b.src;
}

Data parse()
{
  std::ifstream file(std::filesystem::path("inputs/day5.txt"));
  std::string line;
  Data data;

  std::vector<Mapping> ranges;
  while (std::getline(file, line))
  {
    if (line.starts_with("seeds:"))
    {
      data.seeds = parse_numbers(line);
    }
    else
    {
      if (line.find("-to-") != std::string::npos)
      {
        if (ranges.size() > 0)
        {
          data.mappings.push_back(ranges);
          ranges.clear();
        }
      }
      else if (std::isdigit(line[0]))
      {
        auto numbers = parse_numbers(line);
        ranges.push_back({.src = numbers[1], .dest = numbers[0], .length = numbers[2]});
      }
    }
  }
  if (ranges.size() > 0)
  {
    data.mappings.push_back(ranges);
  }

  for (auto &mapping : data.mappings)
  {
    std::sort(mapping.begin(), mapping.end(), compare_mappings);
  }

  return data;
}

class BenchmarkFixture : public benchmark::Fixture
{
public:
  static Data data;
};

Data BenchmarkFixture::data = parse();

BENCHMARK_DEFINE_F(BenchmarkFixture, Part1Benchmark)
(benchmark::State &state)
{
  for (auto _ : state)
  {
    int s = part1(data);
    benchmark::DoNotOptimize(s);
  }
}

BENCHMARK_DEFINE_F(BenchmarkFixture, Part2Benchmark)
(benchmark::State &state)
{
  for (auto _ : state)
  {
    int s = part2(data);
    benchmark::DoNotOptimize(s);
  }
}

BENCHMARK_REGISTER_F(BenchmarkFixture, Part1Benchmark);
BENCHMARK_REGISTER_F(BenchmarkFixture, Part2Benchmark);

int main(int argc, char **argv)
{
  Data data = parse();

  // std::cout << "Part 1: " << part1(data) << std::endl;
  std::cout << "Part 2: " << part2(data) << std::endl;

  // benchmark::Initialize(&argc, argv);
  // benchmark::RunSpecifiedBenchmarks();
}