#include <cstring>
#include <iostream>

#include "arenaAllocator.hpp"
#include "context.hpp"
#include "RamTensor.hpp"
#include "QuantizeOps.hpp"
#include "RomTensor.hpp"

#include "gtest/gtest.h"

#include "constants_quantize.hpp"

using namespace uTensor;
 
TEST(Quantized, reference_0_quantize) {
  localCircularArenaAllocator<1024> meta_allocator;
  localCircularArenaAllocator<784*2*sizeof(float), uint32_t> ram_allocator;
  Context::get_default_context()->set_metadata_allocator(&meta_allocator);
  Context::get_default_context()->set_ram_data_allocator(&ram_allocator);
  Tensor input_tensor = new RomTensor({ 1,28,28,1 }, flt, input_arr);

  Tensor output_tensor = new RamTensor({ 1,28,28,1 }, i8);
  output_tensor->set_quantization_params(PerTensorQuantizationParams(-128, 0.003921568859368563));

  ::TFLM::QuantizeOperator<int8_t, float> op;
  op
    .set_inputs({ { ::TFLM::QuantizeOperator<int8_t, float>::input, input_tensor } })
    .set_outputs({ { ::TFLM::QuantizeOperator<int8_t, float>::output, output_tensor } })
    .eval();
  for (int i = 0; i < 784; ++i) {
    int8_t value = static_cast<int8_t>(output_tensor(i));
    EXPECT_EQ(value, ref_output_arr[i]);
  }
}