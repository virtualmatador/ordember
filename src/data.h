#ifndef SRC_DATA_H
#define SRC_DATA_H

#include <array>
#include <list>
#include <random>
#include <vector>

namespace main {
class Data {
  friend class Menu;
  friend class Game;

  inline static constexpr std::size_t extra_pieces_{2};
  inline static constexpr std::size_t piece_size_{12};

public:
  Data();
  ~Data();
  void load();
  void save() const;
  void reset_all();
  void reset_game();

private:
  bool sound_;
  bool started_;
  int game_over_;
  int hearts_;
  int score_;
  int level_;
  std::vector<std::tuple<int, int, int>> pieces_;
  std::random_device seeder_;
  std::default_random_engine random_;
};

extern Data data_;
} // namespace main

#endif // SRC_DATA_H
