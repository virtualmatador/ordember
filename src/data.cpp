#include <array>
#include <cmath>
#include <iostream>
#include <sstream>

#include "bridge.h"

#include "data.h"

#include "toolbox.hpp"

main::Data main::data_;

main::Data::Data() : random_{seeder_()} {}

main::Data::~Data() {}

void main::Data::load() {
  try {
    int count;
    toolbox::Load("OPTION_SOUND", sound_, false, false);
    toolbox::Load(
        "GAME_OVER",
        *reinterpret_cast<std::underlying_type<Phase>::type *>(&phase_),
        static_cast<std::underlying_type<Phase>::type>(Phase::begin),
        static_cast<std::underlying_type<Phase>::type>(Phase::end));
    toolbox::Load("GAME_LIVES", lives_, 0, max_lives_);
    toolbox::Load("GAME_SCORE", score_, 0, max_score_);
    toolbox::Load("GAME_COUNT", count, 1, max_pieces_);
    pieces_.resize(count);
    for (std::size_t i = 0; i < pieces_.size(); ++i) {
      toolbox::Load(
          (std::ostringstream{} << "GAME_PIECE_S_" << i).str().c_str(),
          std::get<0>(pieces_[i]), false, false);
      toolbox::Load(
          (std::ostringstream{} << "GAME_PIECE_X_" << i).str().c_str(),
          std::get<1>(pieces_[i]), 0, 100);
      toolbox::Load(
          (std::ostringstream{} << "GAME_PIECE_Y_" << i).str().c_str(),
          std::get<2>(pieces_[i]), 0, 100);
    }
  } catch (...) {
    reset_all();
  }
}

void main::Data::save() const {
  toolbox::Save("OPTION_SOUND", sound_);
  toolbox::Save("GAME_OVER", static_cast<std::size_t>(phase_));
  toolbox::Save("GAME_LIVES", lives_);
  toolbox::Save("GAME_SCORE", score_);
  toolbox::Save("GAME_COUNT", pieces_.size());
  for (std::size_t i = 0; i < pieces_.size(); ++i) {
    toolbox::Save((std::ostringstream{} << "GAME_PIECE_S_" << i).str().c_str(),
                  std::get<0>(pieces_[i]));
    toolbox::Save((std::ostringstream{} << "GAME_PIECE_X_" << i).str().c_str(),
                  std::get<1>(pieces_[i]));
    toolbox::Save((std::ostringstream{} << "GAME_PIECE_Y_" << i).str().c_str(),
                  std::get<2>(pieces_[i]));
  }
}

void main::Data::reset_all() {
  sound_ = true;
  lives_ = max_lives_ - 1;
  score_ = 0;
  pieces_.resize(extra_pieces_ + 1);
  reset_game();
}

float main::Data::level_piece_size() const {
  return piece_size_ - static_cast<float>(pieces_.size()) / 2;
}

void main::Data::reset_game() {
  constexpr std::size_t board_size = 100;
  phase_ = Phase::begin;
  const auto current_piece_size = level_piece_size();
  const auto placement_radius = static_cast<int>(std::ceil(current_piece_size));
  std::array<bool, board_size * board_size> fulls{};
  std::size_t full_count = 0;
  const auto fill = [&](std::size_t x, std::size_t y) {
    auto &full = fulls[x * board_size + y];
    if (!full) {
      full = true;
      ++full_count;
    }
  };
  for (std::size_t x = 0; x < board_size; ++x) {
    for (std::size_t y = 0; y < board_size; ++y) {
      if (x == 0 || y == 0 || x >= board_size - placement_radius ||
          y >= board_size - placement_radius) {
        fill(x, y);
      }
    }
  }
  for (std::size_t i = 0; i < pieces_.size(); ++i) {
    auto index = std::uniform_int_distribution<int>(
        0, board_size * board_size - full_count - 1)(random_);
    for (std::size_t x = 0; x < board_size; ++x) {
      for (std::size_t y = 0; y < board_size; ++y) {
        if (!fulls[x * board_size + y]) {
          if (index == 0) {
            pieces_[i] = {true, x, y};
            x = board_size;
            y = board_size;
          } else {
            --index;
          }
        }
      }
    }
    for (std::size_t x =
             std::max(placement_radius - 1, std::get<1>(pieces_[i])) -
             (placement_radius - 1);
         x < std::get<1>(pieces_[i]) + placement_radius; ++x) {
      for (std::size_t y =
               std::max(placement_radius - 1, std::get<2>(pieces_[i])) -
               (placement_radius - 1);
           y < std::get<2>(pieces_[i]) + placement_radius; ++y) {
        const auto delta_x = static_cast<int>(x) - std::get<1>(pieces_[i]);
        const auto delta_y = static_cast<int>(y) - std::get<2>(pieces_[i]);
        if (delta_x * delta_x + delta_y * delta_y <
            current_piece_size * current_piece_size) {
          fill(x, y);
        }
      }
    }
  }
}
