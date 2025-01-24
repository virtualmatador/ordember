#include <iostream>
#include <set>
#include <sstream>

#include "bridge.h"

#include "data.h"

#include "toolbox.hpp"

main::Data main::data_;

main::Data::Data() : random_{seeder_()} {}

main::Data::~Data() {}

void main::Data::load() {
  try {
    toolbox::Load("OPTION_SOUND", sound_, false, false);
    toolbox::Load(
        "GAME_OVER",
        *reinterpret_cast<std::underlying_type<Phase>::type *>(&phase_),
        static_cast<std::underlying_type<Phase>::type>(Phase::begin),
        static_cast<std::underlying_type<Phase>::type>(Phase::end));
    toolbox::Load("GAME_LIVES", lives_, 0, max_lives_);
    toolbox::Load("GAME_SCORE", score_, 0, max_score_);
    toolbox::Load("GAME_LEVEL", level_, 1, max_level_);
    pieces_.resize(level_ + extra_pieces_);
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
  toolbox::Save("GAME_LEVEL", level_);
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
  level_ = 1;
  reset_game();
}

void main::Data::reset_game() {
  phase_ = Phase::begin;
  pieces_.resize(level_ + extra_pieces_);
  std::set<std::tuple<std::size_t, std::size_t>> fulls;
  for (std::size_t x = 100 - Data::piece_size_ + 1; x < 100; ++x) {
    for (std::size_t y = 0; y < 100 - Data::piece_size_ + 1; y++) {
      fulls.insert({x, y});
    }
  }
  for (std::size_t x = 100 - Data::piece_size_ + 1; x < 100; ++x) {
    for (std::size_t y = 100 - Data::piece_size_ + 1; y < 100; y++) {
      fulls.insert({x, y});
    }
  }
  for (std::size_t x = 0; x < 100 - Data::piece_size_ + 1; ++x) {
    for (std::size_t y = 100 - Data::piece_size_ + 1; y < 100; y++) {
      fulls.insert({x, y});
    }
  }
  for (std::size_t i = 0; i < pieces_.size(); ++i) {
    auto index = std::uniform_int_distribution<int>(
        0, 100 * 100 - fulls.size())(random_);
    for (std::size_t x = 0; x < 100; ++x) {
      for (std::size_t y = 0; y < 100; ++y) {
        if (fulls.find({x, y}) == fulls.end()) {
          if (index == 0) {
            pieces_[i] = {true, x, y};
            x = 100;
            y = 100;
          } else {
            --index;
          }
        }
      }
    }
    for (std::size_t x = std::max(piece_size_ - 1, std::get<1>(pieces_[i])) -
                         (piece_size_ - 1);
         x < std::get<1>(pieces_[i]) + piece_size_; ++x) {
      for (std::size_t y = std::max(piece_size_ - 1, std::get<2>(pieces_[i])) -
                           (piece_size_ - 1);
           y < std::get<2>(pieces_[i]) + piece_size_; ++y) {
        fulls.insert({x, y});
      }
    }
  }
}
