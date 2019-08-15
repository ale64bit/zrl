#include "core/Core.h"
#include "core/Log.h"

int main() {
  const zrl::Config config{/* app_name */ "setup",
                           /* engine_name */ "zrl",
                           /* width */ 800,
                           /* height */ 600,
                           /* fullscreen*/ false,
                           /* debug*/ true};
  zrl::Core core(config);
  LOG(INFO) << "Ready!" << std::endl;
  return 0;
}
