#include "anbox/daemon.h"
#include "anbox/utils.h"

int main(int argc, char **argv) {
  anbox::Daemon daemon;
  return daemon.Run(anbox::utils::collect_arguments(argc, argv));
}
