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
    toolbox::Load("OPTION_SOUND", sound_, false, false);
    toolbox::Load("GAME_OVER", game_over_, 0, 3);
    toolbox::Load("GAME_HEARTS", hearts_, 1, 6);
    toolbox::Load("GAME_SCORE", score_, 0, 0);
    toolbox::Load("GAME_LEVEL", level_, 1, 16);
    pieces_.resize(level_ + extra_pieces_);
    for (std::size_t i = 0; i < pieces_.size(); ++i) {
      toolbox::Load(
          (std::ostringstream{} << "GAME_PIECE_S_" << i).str().c_str(),
          std::get<0>(pieces_[i]), 0, 4);
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
  toolbox::Save("GAME_OVER", game_over_);
  toolbox::Save("GAME_HEARTS", hearts_);
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
  sound_ = false;
  hearts_ = 5;
  score_ = 0;
  level_ = 1;
  reset_game();
}

void main::Data::reset_game() {
  game_over_ = 0;
  pieces_.resize(level_ + extra_pieces_);
  std::size_t cells = (100 - piece_size_ + 1) * (100 - piece_size_ + 1);
  for (std::size_t i = 0; i < pieces_.size(); ++i) {
    auto index = std::uniform_int_distribution<int>(0, cells)(random_);
    for (std::size_t x = 0; x < 100 - piece_size_ + 1; ++x) {
      for (std::size_t y = 0; y < 100 - piece_size_ + 1; ++y) {
        bool free = true;
        for (std::size_t p = 0; p < i; ++p) {
          if (x >= std::get<1>(pieces_[p]) &&
              x < std::get<1>(pieces_[p]) + piece_size_ &&
              y >= std::get<2>(pieces_[p]) &&
              y < std::get<2>(pieces_[p]) + piece_size_) {
            free = false;
            break;
          }
        }
        if (free) {
          if (index == 0) {
            pieces_[i] = {0, x, y};
            cells -= std::min(piece_size_, 100 - piece_size_ + 1 - x) *
                     std::min(piece_size_, 100 - piece_size_ + 1 - y);
            x = 100 - piece_size_ + 1;
            y = 100 - piece_size_ + 1;
          } else {
            --index;
          }
        }
      }
    }
  }
}
