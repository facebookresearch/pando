#include "add_arrays.h"
using namespace std;

class FrameAverage {
  // std::deque<std::vector<int>> Aggregator;
  std::vector<int> Aggregator;

 public:
  void init_aggregator(std::vector<short int>);
  void update_aggregator(std::vector<short int>);
};

void FrameAverage::init_aggregator(std::vector<short int> init_frame) {
  update_aggregator = init_frame;
}

void FrameAverage::update_aggregator(std::vector<short int> new_frame) {
  update_aggregator = update_aggregator + new_frame;
}

{
  /*
  for (size_t idx = 0; idx < X; idx++){
      for (size_t idy = 0; idy < Y; idy++){
              ptr4[idx*Y + idy] = std::round((ptr1[idx*Y + idy] + ptr2[idx*Y +
  idy] - ptr3[idx*Y + idy]) / fr_len);
      }
  }
  */

  // reshape to match desired size
  result.resize({X, Y});

  return result;
}
