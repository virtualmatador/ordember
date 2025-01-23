#ifndef SRC_GAME_H
#define SRC_GAME_H

#include "stage.h"

#include "data.h"

namespace main {
class Game : public core::Stage {
public:
  Game();
  ~Game();
  void Escape() override;
  void FeedUri(const char *uri,
               std::function<void(const std::vector<unsigned char> &)>
                   &&consume) override;

private:
  void play_audio(const char *name) const;
  void update_view() const;
  void give_up();
  void game_over() const;
};
} // namespace main

#endif // SRC_GAME_H
