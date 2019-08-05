#include "core/Core.h"
#include "core/Log.h"

int main() {
  const zrl::Config config{/* app_name */ "part2",
                           /* engine_name */ "zrl",
                           /* width */ 1920,
                           /* height */ 1080,
                           /* fullscreen*/ false,
                           /* debug*/ true};
  zrl::Core core(config);
  LOG(INFO) << "Ready!" << std::endl;
  return 0;
}
