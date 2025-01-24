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
      if (data_.phase_ == Data::Phase::begin) {
        if (*info == 'y') {
          reveal();
        } else {
          update_view();
          bridge::CallFunction("countDown(1.0, 'body', 'setup', 'y');");
        }
      } else {
        if (data_.phase_ == Data::Phase::remember) {
          data_.phase_ = Data::Phase::tap;
        }
        update_view();
      }
    } else if (std::strcmp(command, "count") == 0) {
      play_audio("counter");
      auto counter = std::strtol(info, nullptr, 10);
      if (counter > 0) {
        bridge::CallFunction((std::ostringstream{}
                              << "countDown(0.75, 'body', 'count', '"
                              << counter - 1 << "');")
                                 .str()
                                 .c_str());
      } else {
        data_.phase_ = Data::Phase::tap;
        update_view();
      }
    }
  };
  handlers_["piece"] = [&](const char *command, const char *info) {
    if (std::strlen(command) == 0)
      return;
    else if (std::strcmp(command, "tap") == 0) {
      if (data_.phase_ == Data::Phase::tap) {
        auto index = std::strtol(info, nullptr, 10);
        if (std::get<0>(data_.pieces_[index])) {
          bool tapped = true;
          for (std::size_t i = 0; i < index; ++i) {
            if (std::get<0>(data_.pieces_[i])) {
              tapped = false;
              break;
            }
          }
          if (tapped) {
            if (data_.score_ < Data::max_score_ - 1) {
              ++data_.score_;
            }
            std::get<0>(data_.pieces_[index]) = false;
            if (index < data_.pieces_.size() - 1) {
              play_audio("turn");
            } else {
              play_audio("win");
              data_.phase_ = Data::Phase::win;
              if (data_.lives_ < data_.max_lives_ - 1) {
                ++data_.lives_;
              }
            }
            update_view();
          } else {
            loose();
          }
        }
      }
    }
  };
  handlers_["game"] = [&](const char *command, const char *info) {
    if (std::strlen(command) == 0)
      return;
    else if (std::strcmp(command, "stop") == 0) {
      Escape();
    } else if (std::strcmp(command, "go") == 0) {
      switch (data_.phase_) {
      case Data::Phase::begin:
        reveal();
        break;
      case Data::Phase::remember:
        data_.phase_ = Data::Phase::tap;
        update_view();
        break;
      case Data::Phase::tap:
        break;
      case Data::Phase::loose:
        data_.reset_game();
        update_view();
        bridge::CallFunction("countDown(1.0, 'body', 'setup', '');");
        break;
      case Data::Phase::win:
        if (data_.pieces_.size() < data_.max_level_ + Data::extra_pieces_ - 1) {
          data_.pieces_.emplace_back(true, 0, 0);
        }
        data_.reset_game();
        update_view();
        bridge::CallFunction("countDown(1.0, 'body', 'setup', '');");
        break;
      case Data::Phase::over:
        Escape();
        break;
      }
    } else if (std::strcmp(command, "giveup") == 0) {
      if (data_.phase_ < Data::Phase::loose) {
        loose();
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
    std::size_t state;
    switch (data_.phase_) {
    case Data::Phase::begin:
      state = 0;
      break;
    case Data::Phase::remember:
      state = 1;
      break;
    case Data::Phase::tap:
      state = std::get<0>(data_.pieces_[i]) ? 2 : 3;
      break;
    case Data::Phase::loose:
      state = std::get<0>(data_.pieces_[i]) ? 4 : 3;
      break;
    case Data::Phase::win:
      state = 3;
      break;
    case Data::Phase::over:
      state = std::get<0>(data_.pieces_[i]) ? 4 : 3;
      break;
    case Data::Phase::end:
      state = 2;
      break;
    }
    bridge::CallFunction((std::ostringstream{}
                          << "setPiece(" << i << ", " << state << ", "
                          << std::get<1>(data_.pieces_[i]) << ", "
                          << std::get<2>(data_.pieces_[i]) << ");")
                             .str()
                             .c_str());
  }
  const char *color;
  const char *text;
  switch (data_.phase_) {
  case Data::Phase::begin:
    color = "navy";
    text = "Ready";
    break;
  case Data::Phase::remember:
    color = "blue";
    text = "Go";
    break;
  case Data::Phase::tap:
    color = "aqua";
    text = "?";
    break;
  case Data::Phase::loose:
    color = "magenta";
    text = "Wrong!";
    break;
  case Data::Phase::win:
    color = "green";
    text = "Win!";
    break;
  case Data::Phase::over:
    color = "red";
    text = "Game Over!";
    break;
  case Data::Phase::end:
    color = "black";
    text = "";
    break;
  }
  bridge::CallFunction(
      (std::ostringstream{} << "banner('" << color << "', '" << text << "');")
          .str()
          .c_str());
}

void main::Game::loose() {
  play_audio("error");
  if (--data_.lives_ > 0) {
    data_.phase_ = Data::Phase::loose;
  } else {
    data_.phase_ = Data::Phase::over;
  }
  update_view();
}

void main::Game::reveal() {
  data_.phase_ = Data::Phase::remember;
  update_view();
  bridge::CallFunction(
      (std::ostringstream{}
       << "countDown(" << (data_.pieces_.size() - Data::extra_pieces_ - 1) * 0.5
       << ", " << "'body'" << ", " << "'count'" << ", " << "'2'" << ");")
          .str()
          .c_str());
}

void main::Game::FeedUri(
    const char *uri,
    std::function<void(const std::vector<unsigned char> &)> &&consume) {}
