#include "thrift/server.h"
#include "project/project.h"

using namespace om::common;

namespace om {
namespace server {

class serverHandler : virtual public serverIf {
 public:
  serverHandler() {
    // Your initialization goes here
  }

  void get_chan_tile(tile& _return, const vector3d& point) {
    // Your implementation goes here
    printf("get_chan_tile\n");
  }

  void get_seg_tile(tile& _return, const vector3d& point) {
    // Your implementation goes here
    printf("get_seg_tile\n");
  }

  void click(click_info& _return, const vector3d& point) {
    // Your implementation goes here
    printf("click\n");
  }

  double compare_results(const std::vector<result> & old_results, const result & new_result) {
    // Your implementation goes here
    printf("compare_results\n");
    return 0;
  }

};

} // namespace server
} // namespace om
