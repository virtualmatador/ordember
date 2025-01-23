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
      bridge::CallFunction(
          (std::ostringstream{} << "setPieceSize(" << data_.piece_size_ << ");")
              .str()
              .c_str());
      bridge::CallFunction("setup(['counter', 'turn', 'error', 'win']);");
    } else if (std::strcmp(command, "setup") == 0) {
      if (strcmp(info, "y") == 0) {
        if (!data_.started_) {
          data_.started_ = true;
          for (auto &piece : data_.pieces_) {
            std::get<0>(piece) = 1;
          }
          bridge::CallFunction((std::ostringstream{}
                                << "countDown(" << data_.level_ * 0.5 << ", "
                                << "'body'" << ", " << "'count'" << ", "
                                << "'2'" << ");")
                                   .str()
                                   .c_str());
        }
      } else {
        bridge::CallFunction((std::ostringstream{}
                              << "countDown(" << 1.0 << ", " << "'body'" << ", "
                              << "'setup'" << ", " << "'y'" << ");")
                                 .str()
                                 .c_str());
      }
      update_view();
    } else if (std::strcmp(command, "count") == 0) {
      play_audio("counter");
      auto counter = std::strtol(info, nullptr, 10);
      if (counter > 0) {
        bridge::CallFunction((std::ostringstream{}
                              << "countDown(" << 0.75 << ", " << "'body'"
                              << ", " << "'count'" << ", " << "'" << counter - 1
                              << "'" << ");")
                                 .str()
                                 .c_str());
      } else {
        for (auto &piece : data_.pieces_) {
          std::get<0>(piece) = 2;
        }
        update_view();
      }
    }
  };
  handlers_["piece"] = [&](const char *command, const char *info) {
    if (std::strlen(command) == 0)
      return;
    else if (std::strcmp(command, "tap") == 0) {
      if (data_.game_over_ == 0) {
        auto index = std::strtol(info, nullptr, 10);
        if (std::get<0>(data_.pieces_[index]) == 2) {
          bool tapped = true;
          for (std::size_t i = 0; i < index; ++i) {
            if (std::get<0>(data_.pieces_[i]) != 3) {
              std::get<0>(data_.pieces_[i]) = 4;
              tapped = false;
            }
          }
          if (tapped) {
            ++data_.score_;
            std::get<0>(data_.pieces_[index]) = 3;
            if (index < data_.pieces_.size() - 1) {
              play_audio("turn");
            } else {
              play_audio("win");
              data_.game_over_ = 1;
              game_over();
              // TODO increase level
              ++data_.level_;
              // TODO increase hearts
              bridge::CallFunction((std::ostringstream{}
                                    << "countDown(" << 1.5 << ", " << "'game'"
                                    << ", " << "'reset'" << ", " << "'y'"
                                    << ");")
                                       .str()
                                       .c_str());
            }
          } else {
            // TODO decrease hearts
            // TODO decrease levels
            play_audio("error");
            data_.game_over_ = 2;
            game_over();
            for (std::size_t i = index; i < data_.pieces_.size(); ++i) {
              std::get<0>(data_.pieces_[i]) = 4;
            }
          }
          update_view();
        }
      }
    }
  };
  handlers_["game"] = [&](const char *command, const char *info) {
    if (std::strlen(command) == 0)
      return;
    else if (std::strcmp(command, "stop") == 0) {
      for (auto &piece : data_.pieces_) {
        if (std::get<0>(piece) < 2) {
          std::get<0>(piece) = 2;
        }
      }
      Escape();
    } else if (std::strcmp(command, "reset") == 0) {
      data_.reset_game();
      update_view();
      bridge::CallFunction((std::ostringstream{}
                            << "countDown(" << 1.0 << ", " << "'body'" << ", "
                            << "'setup'" << ", " << "''" << ");")
                               .str()
                               .c_str());
    } else if (std::strcmp(command, "giveup") == 0) {
      if (data_.game_over_ == 0) {
        play_audio("error");
        for (auto &piece : data_.pieces_) {
          if (std::get<0>(piece) < 3) {
            std::get<0>(piece) = 4;
          }
        }
        update_view();
        if (strcmp(info, "y") == 0) {
          bridge::CallFunction((std::ostringstream{}
                                << "countDown(" << 1.0 << ", " << "'game'"
                                << ", " << "'reset'" << ", " << "''" << ");")
                                   .str()
                                   .c_str());
        } else {
          data_.game_over_ = 2;
          game_over();
        }
      } else {
        if (strcmp(info, "y") == 0) {
          handlers_["game"]("reset", "");
        }
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

void main::Game::play_audio(const char *name) const {
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

void main::Game::give_up() {}

void main::Game::game_over() const {
  bridge::CallFunction(
      (std::ostringstream{} << "gameOver(" << data_.game_over_ << ");")
          .str()
          .c_str());
}

void main::Game::FeedUri(
    const char *uri,
    std::function<void(const std::vector<unsigned char> &)> &&consume) {}
