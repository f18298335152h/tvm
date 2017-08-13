/*!
 *  Copyright (c) 2017 by Contributors
 * \file Use external nnpack library call.
 */
#include <tvm/runtime/registry.h>
#include <tvm/runtime/util.h>
#include <dmlc/logging.h>
#include <nnpack.h>

namespace tvm {
namespace contrib {
using namespace runtime;

TVM_REGISTER_GLOBAL("tvm.contrib.nnpack.convolution_inference")
.set_body([](TVMArgs args, TVMRetValue *ret) {
    nnp_initialize();
    DLTensor* input  = args[0];
    DLTensor* kernel = args[1];
    DLTensor* bias   = args[2];
    DLTensor* output = args[3];
    nnp_padding input_padding{args[4], args[5], args[6], args[7]};
    nnp_size stride_size{args[8], args[9]};

    CHECK_EQ(input->ndim, 3);
    CHECK_EQ(kernel->ndim, 4);
    CHECK_EQ(bias->ndim, 1);
    CHECK_EQ(output->ndim, 3);

    CHECK_EQ(input->shape[0], kernel->shape[1]);
    size_t input_channels = input->shape[0];
    CHECK_EQ(output->shape[0], kernel->shape[0]);
    CHECK_EQ(output->shape[0], bias->shape[0]);
    size_t output_channels = output->shape[0];
    nnp_size input_size{static_cast<size_t>(input->shape[1]),
                        static_cast<size_t>(input->shape[2])};
    nnp_size kernel_size{static_cast<size_t>(kernel->shape[2]),
                         static_cast<size_t>(kernel->shape[3])};

    CHECK(input->strides == nullptr);
    CHECK(kernel->strides == nullptr);
    CHECK(bias->strides == nullptr);

    CHECK(TypeMatch(input->dtype, kFloat, 32));
    CHECK(TypeMatch(kernel->dtype, kFloat, 32));
    CHECK(TypeMatch(bias->dtype, kFloat, 32));
    CHECK(TypeMatch(output->dtype, kFloat, 32));

    nnp_convolution_inference(nnp_convolution_algorithm_auto,
                              nnp_convolution_transform_strategy_block_based,
                              input_channels,
                              output_channels,
                              input_size,
                              input_padding,
                              kernel_size,
                              stride_size,
                              static_cast<float*>(input->data),
                              static_cast<float*>(kernel->data),
                              static_cast<float*>(bias->data),
                              static_cast<float*>(output->data),
                              NULL,
                              NULL,
                              nnp_activation_identity,
                              NULL,
                              NULL,
                              NULL);
  });


TVM_REGISTER_GLOBAL("tvm.contrib.nnpack.convolution_output")
.set_body([](TVMArgs args, TVMRetValue *ret) {
    nnp_initialize();
    DLTensor* input  = args[0];
    DLTensor* kernel = args[1];
    DLTensor* bias   = args[2];
    DLTensor* output = args[3];
    nnp_padding input_padding{args[4], args[5], args[6], args[7]};

    CHECK_EQ(input->ndim, 4);
    CHECK_EQ(kernel->ndim, 4);
    CHECK_EQ(bias->ndim, 1);
    CHECK_EQ(output->ndim, 4);

    CHECK_EQ(input->shape[0], output->shape[0]);
    size_t batch_size = input->shape[0];
    CHECK_EQ(input->shape[1], kernel->shape[1]);
    size_t input_channels = input->shape[1];
    CHECK_EQ(output->shape[1], bias->shape[0]);
    CHECK_EQ(output->shape[1], kernel->shape[0]);
    size_t output_channels = output->shape[1];
    nnp_size input_size{static_cast<size_t>(input->shape[2]),
                        static_cast<size_t>(input->shape[3])};
    nnp_size kernel_size{static_cast<size_t>(kernel->shape[2]),
                         static_cast<size_t>(kernel->shape[3])};

    CHECK(input->strides == nullptr);
    CHECK(kernel->strides == nullptr);
    CHECK(bias->strides == nullptr);

    CHECK(TypeMatch(input->dtype, kFloat, 32));
    CHECK(TypeMatch(kernel->dtype, kFloat, 32));
    CHECK(TypeMatch(bias->dtype, kFloat, 32));
    CHECK(TypeMatch(output->dtype, kFloat, 32));

    nnp_convolution_output(nnp_convolution_algorithm_auto,
                           batch_size,
                           input_channels,
                           output_channels,
                           input_size,
                           input_padding,
                           kernel_size,
                           static_cast<float*>(input->data),
                           static_cast<float*>(kernel->data),
                           static_cast<float*>(bias->data),
                           static_cast<float*>(output->data),
                           nnp_activation_identity,
                           NULL,
                           NULL,
                           NULL);
  });
}  // namespace contrib
}  // namespace tvm