#include <cstring>
#include <sstream>

#include "helper.h"

#include "data.h"
#include "menu.h"
#include "progress.h"

main::Menu::Menu() {
  handlers_["body"] = [&](const char *command, const char *info) {
    if (std::strlen(command) == 0)
      return;
    else if (std::strcmp(command, "ready") == 0) {
      bridge::CallFunction(
          (std::ostringstream{} << "setLives(" << data_.lives_ << ");")
              .str()
              .c_str());
      bridge::CallFunction(
          (std::ostringstream{} << "setLevel(" << data_.level_ << ");")
              .str()
              .c_str());
      bridge::CallFunction(
          (std::ostringstream{} << "setScore(" << data_.score_ << ");")
              .str()
              .c_str());
      bridge::CallFunction((std::ostringstream{}
                            << "setSound(" << (data_.sound_ ? "true" : "false")
                            << ");")
                               .str()
                               .c_str());
    }
  };
  handlers_["play"] = [&](const char *command, const char *info) {
    if (std::strlen(command) == 0)
      return;
    else if (std::strcmp(command, "click") == 0)
      Play();
  };
  handlers_["sound"] = [&](const char *command, const char *info) {
    if (std::strlen(command) == 0)
      return;
    else if (std::strcmp(command, "click") == 0) {
      if (std::strlen(info) == 0) {
        return;
      } else if (std::strcmp(info, "true") == 0) {
        data_.sound_ = true;
      } else if (std::strcmp(info, "false") == 0) {
        data_.sound_ = false;
      }
    }
  };
  handlers_["reset"] = [&](const char *command, const char *info) {
    if (std::strlen(command) == 0) {
      return;
    } else if (std::strcmp(command, "click") == 0) {
      data_.reset_all();
      bridge::NeedRestart();
    }
  };
  bridge::LoadView(index_,
                   (std::int32_t)core::VIEW_INFO::Default |
                       (std::int32_t)core::VIEW_INFO::AudioNoSolo,
                   "menu");
}

main::Menu::~Menu() {}

void main::Menu::Escape() { bridge::Exit(); }

void main::Menu::Play() {
  progress_ = PROGRESS::GAME;
  bridge::NeedRestart();
}

void main::Menu::FeedUri(
    const char *uri,
    std::function<void(const std::vector<unsigned char> &)> &&consume) {}
