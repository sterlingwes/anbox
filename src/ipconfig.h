#ifndef ANBOX_DAEMON_H_
#define ANBOX_DAEMON_H_

#include <string>
#include <vector>

#include "anbox/cli.h"
#include "anbox/do_not_copy_or_move.h"

namespace anbox {
class Daemon : public DoNotCopyOrMove {
 public:
  Daemon();

  int Run(const std::vector<std::string> &arguments);
};
}  // namespace anbox

#endif
