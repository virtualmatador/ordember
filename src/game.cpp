#include <cstring>
#include <sstream>

#include "helper.h"

#include "game.h"
#include "progress.h"

main::Game::Game() {
  handlers_["body"] = [&](const char *command, const char *info) {
    if (std::strlen(command) == 0)
      return;
    else if (std::strcmp(command, "ready") == 0) {
      bridge::CallFunction("setup(['counter', 'turn', 'error', 'win']);");
    } else if (std::strcmp(command, "setup") == 0) {
      update_view();
    }
  };
  handlers_["piece"] = [&](const char *command, const char *info) {
    if (std::strlen(command) == 0)
      return;
    else if (std::strcmp(command, "click") == 0) {
      if (data_.game_over_ == 0) {
        play_audio('turn');
      }
    }
  };
  handlers_["game"] = [&](const char *command, const char *info) {
    if (std::strlen(command) == 0)
      return;
    else if (std::strcmp(command, "stop") == 0) {
      Escape();
    } else if (std::strcmp(command, "reset") == 0) {
      if (data_.game_over_ == 0) {
        play_audio('error');
      }
      data_.reset_game();
      update_view();
    } else if (std::strcmp(command, "giveup") == 0) {
      if (data_.game_over_ == 0) {
        give_up();
      }
    }
  };
  bridge::LoadView(index_,
                   (std::int32_t)core::VIEW_INFO::Portrait |
                       (std::int32_t)core::VIEW_INFO::AudioNoSolo,
                   "game");
}

main::Game::~Game() {}

void main::Game::Escape() {
  main::progress_ = PROGRESS::MENU;
  bridge::NeedRestart();
}

void main::Game::play_audio(const char name) const {
  if (data_.sound_) {
    bridge::CallFunction(
        (std::ostringstream{} << "playAudio('" << name << "');").str().c_str());
  }
}

void main::Game::update_view() const {
  bridge::CallFunction(
      (std::ostringstream{} << "resize(" << data_.pieces_.size() << ");")
          .str()
          .c_str());
  for (std::size_t i = 0; i < data_.pieces_.size(); ++i) {
    bridge::CallFunction((std::ostringstream{}
                          << "setPiece(" << i << ", "
                          << std::get<0>(data_.pieces_[i]) << ", "
                          << std::get<1>(data_.pieces_[i]) << ", "
                          << std::get<2>(data_.pieces_[i]) << ");")
                             .str()
                             .c_str());
  }
  game_over();
}

void main::Game::give_up() {
  for (auto &piece : data_.pieces_) {
    if (std::get<0>(piece) < 2) {
      std::get<0>(piece) = 3;
    }
  }
  data_.game_over_ = 2;
  game_over();
  play_audio('error');
}

void main::Game::game_over() const {
  bridge::CallFunction(
      (std::ostringstream{} << "gameOver(" << data_.game_over_ << ");")
          .str()
          .c_str());
}

void main::Game::FeedUri(
    const char *uri,
    std::function<void(const std::vector<unsigned char> &)> &&consume) {}
