#ifndef SRC_DATA_H
#define SRC_DATA_H

#include <array>
#include <list>
#include <random>
#include <vector>

namespace main {
class Data {
private:
  friend class Menu;
  friend class Game;

  enum class Phase {
    begin,
    remember,
    tap,
    loose,
    win,
    over,
    end,
  };

  inline static constexpr int extra_pieces_{2};
  inline static constexpr int piece_size_{15};
  inline static constexpr int max_level_{16};
  inline static constexpr int max_lives_{6};
  inline static constexpr int max_score_{1 << 30};
  inline static constexpr int max_state_{1 << 10};

public:
  Data();
  ~Data();
  void load();
  void save() const;
  void reset_all();
  void reset_game();

private:
  bool sound_;
  Phase phase_;
  int lives_;
  int score_;
  std::vector<std::tuple<bool, int, int>> pieces_;
  std::random_device seeder_;
  std::default_random_engine random_;
};

extern Data data_;
} // namespace main

#endif // SRC_DATA_H
