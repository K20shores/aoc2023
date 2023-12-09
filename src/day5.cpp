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
  // std::cout << "\n-----\n";
  std::vector<Range> ranges = {first_range};
  for (const auto &mappings : data.mappings)
  {
    // for (auto &range : ranges)
    // {
    //   std::cout << "start " << range.start << " end " << range.end << " length " << range.size << std::endl;
    // }
    // std::cout << std::endl;
    std::vector<Range> new_ranges;
    for (auto& range: ranges)
    {
      for (const auto &mapping : mappings)
      {
        // mapping: |             [      )
        // range:   |     [          )
        if (mapping.src >= range.start && mapping.src < range.end)
        {
          // map the overlap
          auto size = std::min(mapping.length, range.end - mapping.src);
          auto end = mapping.dest + size;

          if (size < range.size)
          {
            // there was a partial overlap
            Range r = {.start = mapping.dest, .end = end, .size = size};

            if (range.end > mapping.src + mapping.length)
            {
              // the range did go past the end of the mapping
              auto start = mapping.src + mapping.length;
              Range r2 = {.start = start, .end = range.end, .size = range.end - start};
              range.end = mapping.src;
              range.size = range.end - range.start;
              new_ranges.push_back(r2);
            }
            else
            {
              // the range didn't go past the end of the mapping
              // update the range
              range.end -= r.size;
              range.size -= r.size;
            }

            new_ranges.push_back(r);
          }
          else
          {
            // the range was entirely within the mapping
            range.start = mapping.dest;
            range.end = end;
            range.size = size;
          }
        }
        // range:   |             [      )
        // mapping: |     [          )
        else if (range.start >= mapping.src && range.start <= (mapping.src + mapping.length))
        {
          // map the overlap
          auto start = mapping.dest + (range.start - mapping.src);
          auto size = std::min(range.size, mapping.src + mapping.length - range.start);
          auto end = start + size;

          if (size < range.size)
          {
            // there was a partial overlap
            Range r = {.start = start, .end = end, .size = end - start};

            // update the range
            range.start += r.size;
            range.size -= r.size;

            new_ranges.push_back(r);
          }
          else
          {
            // the range was entirely within the mapping
            range.start = start;
            range.end = end;
          }
        }
      }
    }
    ranges.insert(ranges.end(), new_ranges.begin(), new_ranges.end());
  }
  // for (auto &range : ranges)
  // {
  //   std::cout << "start " << range.start << " end " << range.end << " length " << range.size << std::endl;
  // }
  // std::cout << std::endl;
  auto p = 0;
  long min = std::numeric_limits<long>::max();
  for (const auto &range : ranges)
  {
    min = std::min(min, range.start);
    p += range.size;
  }
  if (p != s)
    throw std::runtime_error("sizes not equal");
  // std::cout << min << std::endl;
  // std::cout << "\n=====\n";
  return min;
}

long old_part1(const Data &data)
{
  long min = LONG_MAX;
  for (const auto &seed : data.seeds)
  {
    long location = seed;
    for (const auto &ranges : data.mappings)
    {
      for (const auto &range : ranges)
      {
        if (location >= range.src && location < range.src + range.length)
        {
          location = (location - range.src) + range.dest;
          break;
        }
        if (location < range.src)
        {
          break;
        }
      }
    }
    min = std::min(min, location);
  }
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
  return a.dest < b.dest;
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

  std::cout << "Part 1: " << part1(data) << std::endl;
  std::cout << "Part 1: " << old_part1(data) << std::endl;
  // std::cout << "Part 2: " << part2(data) << std::endl;

  // benchmark::Initialize(&argc, argv);
  // benchmark::RunSpecifiedBenchmarks();
}