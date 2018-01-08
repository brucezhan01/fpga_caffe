#include "fpga_caffe/test/test_fpga_caffe_main.hpp"
#include <stdarg.h>
#include <stdio.h> 
#include <unistd.h> // readlink, chdir

void printError(cl_int error) {
  // Print error message
  switch(error)
  {
    case 0:
      break;
    case -1:
      printf("CL_DEVICE_NOT_FOUND ");
      break;
    case -2:
      printf("CL_DEVICE_NOT_AVAILABLE ");
      break;
    case -3:
      printf("CL_COMPILER_NOT_AVAILABLE ");
      break;
    case -4:
      printf("CL_MEM_OBJECT_ALLOCATION_FAILURE ");
      break;
    case -5:
      printf("CL_OUT_OF_RESOURCES ");
      break;
    case -6:
      printf("CL_OUT_OF_HOST_MEMORY ");
      break;
    case -7:
      printf("CL_PROFILING_INFO_NOT_AVAILABLE ");
      break;
    case -8:
      printf("CL_MEM_COPY_OVERLAP ");
      break;
    case -9:
      printf("CL_IMAGE_FORMAT_MISMATCH ");
      break;
    case -10:
      printf("CL_IMAGE_FORMAT_NOT_SUPPORTED ");
      break;
    case -11:
      printf("CL_BUILD_PROGRAM_FAILURE ");
      break;
    case -12:
      printf("CL_MAP_FAILURE ");
      break;
    case -13:
      printf("CL_MISALIGNED_SUB_BUFFER_OFFSET ");
      break;
    case -14:
      printf("CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST ");
      break;

    case -30:
      printf("CL_INVALID_VALUE ");
      break;
    case -31:
      printf("CL_INVALID_DEVICE_TYPE ");
      break;
    case -32:
      printf("CL_INVALID_PLATFORM ");
      break;
    case -33:
      printf("CL_INVALID_DEVICE ");
      break;
    case -34:
      printf("CL_INVALID_CONTEXT ");
      break;
    case -35:
      printf("CL_INVALID_QUEUE_PROPERTIES ");
      break;
    case -36:
      printf("CL_INVALID_COMMAND_QUEUE ");
      break;
    case -37:
      printf("CL_INVALID_HOST_PTR ");
      break;
    case -38:
      printf("CL_INVALID_MEM_OBJECT ");
      break;
    case -39:
      printf("CL_INVALID_IMAGE_FORMAT_DESCRIPTOR ");
      break;
    case -40:
      printf("CL_INVALID_IMAGE_SIZE ");
      break;
    case -41:
      printf("CL_INVALID_SAMPLER ");
      break;
    case -42:
      printf("CL_INVALID_BINARY ");
      break;
    case -43:
      printf("CL_INVALID_BUILD_OPTIONS ");
      break;
    case -44:
      printf("CL_INVALID_PROGRAM ");
      break;
    case -45:
      printf("CL_INVALID_PROGRAM_EXECUTABLE ");
      break;
    case -46:
      printf("CL_INVALID_KERNEL_NAME ");
      break;
    case -47:
      printf("CL_INVALID_KERNEL_DEFINITION ");
      break;
    case -48:
      printf("CL_INVALID_KERNEL ");
      break;
    case -49:
      printf("CL_INVALID_ARG_INDEX ");
      break;
    case -50:
      printf("CL_INVALID_ARG_VALUE ");
      break;
    case -51:
      printf("CL_INVALID_ARG_SIZE ");
      break;
    case -52:
      printf("CL_INVALID_KERNEL_ARGS ");
      break;
    case -53:
      printf("CL_INVALID_WORK_DIMENSION ");
      break;
    case -54:
      printf("CL_INVALID_WORK_GROUP_SIZE ");
      break;
    case -55:
      printf("CL_INVALID_WORK_ITEM_SIZE ");
      break;
    case -56:
      printf("CL_INVALID_GLOBAL_OFFSET ");
      break;
    case -57:
      printf("CL_INVALID_EVENT_WAIT_LIST ");
      break;
    case -58:
      printf("CL_INVALID_EVENT ");
      break;
    case -59:
      printf("CL_INVALID_OPERATION ");
      break;
    case -60:
      printf("CL_INVALID_GL_OBJECT ");
      break;
    case -61:
      printf("CL_INVALID_BUFFER_SIZE ");
      break;
    case -62:
      printf("CL_INVALID_MIP_LEVEL ");
      break;
    case -63:
      printf("CL_INVALID_GLOBAL_WORK_SIZE ");
      break;
    default:
      printf("UNRECOGNIZED ERROR CODE (%d)", error);
  }
}

// Returns the platform name.
std::string getPlatformName(cl_platform_id pid) {
  cl_int status;

  size_t sz;
  status = clGetPlatformInfo(pid, CL_PLATFORM_NAME, 0, NULL, &sz);
  if (status != CL_SUCCESS) {
    std::cout << "Query for platform name size failed" << std::endl;
  }

  //scoped_array<char> name(sz);
  char name[256];
  status = clGetPlatformInfo(pid, CL_PLATFORM_NAME, sz, name, NULL);
  if (status != CL_SUCCESS) {
      std::cout << "Query for platform name failed" << std::endl;
  }

  //return name.get();
  return name;
}


std::string getDeviceName(cl_device_id did) {
  cl_int status;

  size_t sz;
  status = clGetDeviceInfo(did, CL_DEVICE_NAME, 0, NULL, &sz);
  if (status != CL_SUCCESS) {
      std::cout << "Failed to get device name size" << std::endl;
  }

  //scoped_array<char> name(sz);
  char name[256];
  status = clGetDeviceInfo(did, CL_DEVICE_NAME, sz, name, NULL);
  if (status != CL_SUCCESS) {
      std::cout << "Failed to get device name" << std::endl;
  }

  //return name.get();
  return name;
}

OCLUtil::OCLUtil(std::string xclbin, std::string xclkernel) {
  xcl_name = xclbin;
  kernel = xclkernel;
}

void OCLUtil::Setup_Platform() {
  oclPlatform.resize(1);
  clGetPlatformIDs(0, NULL, &oclNumPlatforms);
  clGetPlatformIDs(1, &(oclPlatform[0]), NULL);
  clGetDeviceIDs(oclPlatform[0], CL_DEVICE_TYPE_ACCELERATOR, 1, &oclDevices,
      NULL);
  oclContext = clCreateContext(NULL, 1, &oclDevices, NULL, NULL, NULL);
  oclCommandQueue = clCreateCommandQueue(oclContext, oclDevices,
      /*CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE*/ 0, NULL);
}

void OCLUtil::Setup() {
    std::cout << "OCLUtil::Setup()" << std::endl;
  std::string path(".build_release/opencl/src/caffe/layers/");
  std::string temp = path + xcl_name;
  std::cout << "Binary file name is " << temp << std::endl;
  const char *filename = temp.c_str();
  std::ifstream file_stream(filename);
  std::string source( (std::istreambuf_iterator<char>(file_stream)),
      (std::istreambuf_iterator<char>()));
  size_t sourceSize = source.length();
 
  const char *sourceStr = source.c_str();
  cl_int status;

  oclPlatform.resize(1);
  status = clGetPlatformIDs(0, NULL, &oclNumPlatforms);
  if (status != CL_SUCCESS) {
      std::cout << "Query for number of platforms failed" << std::endl;
  }
  status = clGetPlatformIDs(1, &(oclPlatform[0]), NULL);
  if (status != CL_SUCCESS) {
      std::cout << "Query for all platform ids failed" << std::endl;
  }
  std::string platform_name = getPlatformName(oclPlatform[0]);
  std::cout << "Platform: " << platform_name << std::endl;
  cl_uint num_devices;
  status = clGetDeviceIDs(oclPlatform[0], CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
  if (status != CL_SUCCESS) {
      std::cout << "Query for number of devices failed" << std::endl;
  }
  std::cout << "Found " << num_devices << " devices" << std::endl;
  status = clGetDeviceIDs(oclPlatform[0], CL_DEVICE_TYPE_ACCELERATOR, 1, &oclDevices,
      NULL);
  if (status != CL_SUCCESS) {
      std::cout << "Query device type accelerator failed" << std::endl;
  }
  std::string device_name = getDeviceName(oclDevices);
  std::cout << "Device name: " << device_name << std::endl;
  oclContext = clCreateContext(NULL, 1, &oclDevices, NULL, NULL, &status);
  if (status != CL_SUCCESS) {
      std::cout << "Create context failed" << std::endl;
  }
  oclCommandQueue = clCreateCommandQueue(oclContext, oclDevices,
      /*CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE*/ 0, NULL);
  cl_int binary_status;
  cl_program oclProgram = clCreateProgramWithBinary(oclContext, 1,
      &oclDevices, &sourceSize, (const unsigned char **)(&sourceStr), &binary_status,
      &status);
  if (status != CL_SUCCESS) {
      std::cout << "Create program failed" << std::endl;
      std::cout << "source size is " << sourceSize << std::endl;
  }
  if (binary_status != CL_SUCCESS) {
      std::cout << "Failed to load binary for device" << std::endl;
  }
  clBuildProgram(oclProgram, 0, NULL, NULL, NULL, NULL);
  oclKernel = clCreateKernel(oclProgram, kernel.c_str(), NULL);
}

void fillVector(std::vector<float>& input, float beg, float end) {
  static boost::random::mt19937 engine(time(0));
  boost::random::uniform_real_distribution<float> dis(beg, end);
  std::vector<float>::iterator it;
  for (it = input.begin(); it < input.end(); ++it) {
    *it = dis(engine);
  }
}

void fillVectorCPFP(std::vector<float>& input, float beg, float end) {
  static boost::random::mt19937 engine(time(0));
  boost::random::uniform_real_distribution<float> dis(beg, end);
  std::vector<float>::iterator it;
  for (it = input.begin(); it < input.end(); ++it) {
    *it = float(cpfp(dis(engine)));
  }
}

void copyVector(std::vector<float> input, std::vector<float>& output,
    int xsize, int xsize_pad) {
  for (int i = 0; i < output.size() / xsize_pad; ++i) {
    for (int j = 0; j < xsize_pad; ++j) {
      if (j < xsize) 
        output[i * xsize_pad + j] = input[i * xsize + j];
      else
        output[i * xsize_pad + j] = 0;
    }
  }
}

void copyWeights(std::vector<float> w_input, std::vector<float>& w_output,
    int ksize, int padsize, kernel_params params) {
  int oc = params.outchannels * params.numgroups;
  int ic = params.inchannels;
  int bc = params.burstchannels;
  for (int i = 0; i < oc; ++i) {
    for (int n = 0; n < ic / bc; ++n) {
      for (int m = 0; m < bc; ++m) {
        int out_idx = (i * ic + n * bc + m) * padsize;
        int in_idx = (i * ic + n * bc + m) * ksize * ksize;
        if (ksize == 1) {
          int bc_mod = (bc % 16 == 0) ? bc : (bc / 16 + 1) * 16;
          out_idx = (i * bc_mod * (ic / bc) + n * bc_mod + m) * padsize;
          in_idx = (i * ic + n * bc + m) * ksize * ksize;
          w_output[out_idx] = w_input[in_idx];
        } else if (ksize == 3) {
          for (int j = 0; j < ksize * ksize; ++j) {
            w_output[out_idx + j] = w_input[in_idx + j];
          }
        } else if (ksize == 5) {
          for (int j = 0; j < 5; ++j) {
            for (int k = 0; k < 3; ++k) {
              w_output[out_idx + j * 3 + k] = w_input[in_idx + j * 5 + k];
              if (k < 2) 
                w_output[out_idx + 16 + j * 3 + k] =
                  w_input[in_idx + j * 5 + 3 + k];
              else
                w_output[out_idx + 16 + j * 3 + k] = 0;
            }
          } 
        }
      }
    }
  }
}

void toCPFP(std::vector<float> input, std::vector<cpfp>& output) {
  for (int i = 0; i < input.size(); ++i) {
    output[i] = cpfp(input[i]);
  } 
}

void toFloat(std::vector<cpfp> input, std::vector<float>& output) {
  for (int i = 0; i < input.size(); ++i) {
    output[i] = float(input[i]);
  }
}

void ref_fc_layer(std::vector<float> input, std::vector<float> weights,
    std::vector<float> bias, std::vector<float>& output,
    kernel_params params) {
  int inchannels = params.xtile_pad * 2;
  int outchannels = params.outchannels;

  for (int n = 0; n < params.numimages; ++n) {
    for (int j = 0; j < outchannels; ++j)
      output[n * outchannels + j] = bias[j];

    for (int i = 0; i < inchannels; ++i) {
      for (int j = 0; j < outchannels; ++j) {
        output[n * outchannels + j] += input[n * inchannels + i] *
          weights[j * inchannels + i];
      }
    }
  }
}

void ref_backward_fc_layer(std::vector<float> input,
    std::vector<float> weights, std::vector<float>& output,
    kernel_params params) {
  int inchannels = params.xtile_pad * 2;
  int outchannels = params.outchannels;

  for (int n = 0; n < params.numimages; ++n) {
    for (int i = 0; i < inchannels; ++i) {
      for (int j = 0; j < outchannels; ++j) {
        output[j * inchannels + i] += input[i * params.numimages + n] *
          weights[j * params.numimages + n];
      }
    }
  }
}

void ref_relu_layer(std::vector<float>& output) {
  for (int i = 0; i < output.size(); ++i)
    output[i] = std::max((float)0.0, output[i]);
}

void ref_pool_layer_hwcn(std::vector<float> input, std::vector<float>& output,
    std::vector<short>& sw_relu_vals, kernel_params params) {

  for (int i = 0; i < output.size(); ++i)
    output[i] = -FLT_MAX;

  int pooled_height_ = static_cast<int>(ceil(static_cast<float>(
       params.ydim - params.pksize) / 2)) + 1;
  int pooled_width_ = pooled_height_;
  int kernel_h_ = params.pksize;
  int kernel_w_ = params.pksize;
  int height_ = params.ydim;
  int width_ = params.xdim;
  int channels_ = params.inchannels;
  int num_ = params.numimages;
  int stride_h_ = 2;
  int stride_w_ = 2;
 
  for (int ph = 0; ph < pooled_height_; ++ph) {
    for (int pw = 0; pw < pooled_width_; ++pw) {
      int hstart = ph * stride_h_;
      int wstart = pw * stride_w_;
      int hend = std::min(hstart + kernel_h_, height_);
      int wend = std::min(wstart + kernel_w_, width_);
      hstart = std::max(hstart, 0);
      wstart = std::max(wstart, 0);
      for (int h = hstart; h < hend; ++h) {
        for (int w = wstart; w < wend; ++w) {
          for (int c = 0; c < channels_; ++c) {
            for (int n = 0; n < num_; ++n) {
              int top_offset = ((ph * pooled_width_ + pw) * channels_ + c)
                * num_ + n;
              int index = h * width_ + w;
              int bot_offset = (index * channels_ + c) * num_ + n;
              if (input[bot_offset] > output[top_offset]) {
                output[top_offset] = input[bot_offset];
                sw_relu_vals[top_offset] = (h - hstart) * 3 + (w - wstart);
              }
            }
          }
        }
      }
    }
  }
}

void ref_backward_pool_layer_hwcn(std::vector<float> input,
    std::vector<float>& output, std::vector<short> relu_vals,
    kernel_params params) {

  for (int i = 0; i < output.size(); ++i)
    output[i] = 0;

  int pooled_height_ = static_cast<int>(ceil(static_cast<float>(
       params.ydim - params.pksize) / 2)) + 1;
  int pooled_width_ = pooled_height_;
  int width_ = params.xdim;
  int channels_ = params.inchannels;
  int num_ = params.numimages;
  int stride_h_ = 2;
  int stride_w_ = 2;
 
  for (int ph = 0; ph < pooled_height_; ++ph) {
    for (int pw = 0; pw < pooled_width_; ++pw) {
      for (int c = 0; c < channels_; ++c) {
        for (int n = 0; n < num_; ++n) {
          int in_idx = ((ph * pooled_width_ + pw) * channels_ + c) * num_ + n;
          int hw = relu_vals[in_idx];
          int hstart = ph * stride_h_;
          int wstart = pw * stride_w_;
          int w = hw % 3;
          int h = hw / 3;
          int bottom_index = (((hstart + h) * width_ + (wstart + w))
              * channels_ + c) * num_ + n;
          output[bottom_index] += input[in_idx];
        }
      }
    }
  }
}

void ref_conv_layer_hwcn(std::vector<float> input, std::vector<float> weights,
    std::vector<float> bias, std::vector<float>& output, kernel_params params,
    bool wino) {
  int o_head, k_head;
  int out_idx, in_idx, k_idx;

  int numgroups = params.numgroups;
  int inchannels = params.inchannels * numgroups;
  int outchannels = params.outchannels * numgroups;
  int numimages = params.numimages;
  int ksize = params.ksize;
  int ydim = params.ydim;
  int xdim = params.xdim;
  int outYDim = (params.ydim - params.ksize + 2 * params.pad) /
    params.stride + 1;
  int outXDim = (params.xdim - params.ksize + 2 * params.pad) /
    params.stride + 1;

  int pad = params.pad;
  int stride = params.stride;

  int burstoc = params.burstydim;
  int rpofm = params.rpofm;
  int burstchannels = params.burstchannels;
  int rpo = params.rpo;
  // Convolution
  for (int n = 0; n < numimages; n++) {
    for (int g = 0; g < numgroups; g++) {
      o_head = (outchannels / numgroups) * g;
      k_head = (inchannels / numgroups) * g;
      int o_g = outchannels / numgroups;
      int k_g = inchannels / numgroups;
      for (int o = 0; o < rpofm; o++) {
        for (int b = 0; b < burstoc; b++) {
          for (int r = 0; r < rpo; ++r) {
            for (int k = 0; k < burstchannels / 4; k++) {
              for (int m = 0; m < 4; ++m) {
                for (int y = 0; y < outYDim; y++) {
                  for (int x = 0; x < outXDim; x++) {
                    for (int p = 0; p < ksize; p++) {
                      for (int q = 0; q < ksize; q++) {
                        int in_y = y * stride - pad + p;
                        int in_x = x * stride - pad + q;
                        if (in_y >= 0 && in_y < ydim && in_x >= 0 &&
                            in_x < xdim && o * burstoc + b < o_g) {
                          out_idx = ((y * outXDim + x) * outchannels +
                              (o * burstoc + b) + o_head) * numimages + n;
                          in_idx = ((in_y * xdim + in_x) * inchannels + m *
                            (burstchannels / 4) + r * burstchannels + k +
                            k_head) * numimages + n;
                          if (wino) {
                            int burst_idx = p * burstchannels + k * 4 + m +
                              b * ksize * burstchannels;
                            k_idx = (q * o_g + o_head + o * burstoc) * ksize *
                              k_g + r * burstchannels * ksize * burstoc +
                              burst_idx;
                          } else {
                            int burst_idx = (p * ksize + q) * burstchannels +
                              k * 4 + m + b * ksize * ksize * burstchannels;
                            k_idx = (o_head + o * burstoc) * ksize * ksize *
                              k_g + r * burstchannels * ksize * ksize *
                              burstoc + burst_idx;
                          }
                          output[out_idx] += input[in_idx] * weights[k_idx];
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  for (int n = 0; n < numimages; n++) {
    for (int o = 0; o < outchannels; ++o) {
      for (int y = 0; y < outYDim; ++y) {
        for (int x = 0; x < outXDim; ++x) {
          out_idx = ((y * outXDim + x) * outchannels + o) * numimages + n;
          output[out_idx] += bias[o];
        }
      }
    }
  }
}

void ref_backward_conv_layer_hwcn(std::vector<float> input,
    std::vector<float> weights, std::vector<float>& output,
    kernel_params params) {
  int o_head, k_head;
  int out_idx, in_idx, k_idx;

  int numgroups = params.numgroups;
  int inchannels = params.inchannels * numgroups;
  int outchannels = params.outchannels * numgroups;
  int numimages = params.numimages;
  int ksize = params.ksize;

  int pad = params.pad;
  int stride = params.stride;
  int ydim = params.ydim;
  int xdim = params.xdim;
  int outYDim = (params.ydim - params.ksize + 2 * params.pad) /
    params.stride + 1;
  int outXDim = (params.xdim - params.ksize + 2 * params.pad) /
    params.stride + 1;
  int burstoc = params.burstydim;
  int rpofm = params.rpofm;
  int burstchannels = params.burstchannels;
  int rpo = params.rpo;

  for (int n = 0; n < numimages; n++) {
    for (int g = 0; g < numgroups; g++) {
      o_head = (outchannels / numgroups) * g;
      k_head = (inchannels / numgroups) * g;
      int o_g = outchannels / numgroups;
      int k_g = inchannels / numgroups;
      for (int o = 0; o < rpofm; o++) {
        for (int b = 0; b < burstoc; b++) {
          for (int r = 0; r < rpo; ++r) {
            for (int k = 0; k < burstchannels / 4; k++) {
              for (int m = 0; m < 4; ++m) {
                for (int p = 0; p < outYDim; p++) {
                  for (int q = 0; q < outXDim; q++) {
                    for (int y = 0; y < ksize; y++) {
                      for (int x = 0; x < ksize; x++) {
                        int in_y = p * stride - pad + y;
                        int in_x = q * stride - pad + x;
                        if (in_y >= 0 && in_y < ydim
                          && in_x >= 0 && in_x < xdim
                          && o * burstoc + b < o_g) {
                          out_idx = ((p * outXDim + q) * outchannels +
                              (o * burstoc + b) + o_head) * numimages + n;
                          in_idx = ((in_y * xdim + in_x) * inchannels + m *
                            (burstchannels / 4) + r * burstchannels + k +
                            k_head) * numimages + n;
                          int burst_idx = (y * ksize + x) * burstchannels +
                            k * 4 + m + b * ksize * ksize * burstchannels;
                          k_idx = (o_head + o * burstoc) * ksize * ksize * k_g
                            + r * burstchannels * ksize * ksize * burstoc +
                            burst_idx;

//                          k_idx = (((o + o_head) * ksize + y) * ksize + x) *
//                            k_g + k * 4 + m;
                          output[k_idx] += input[in_idx] * weights[out_idx];
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

bool checkEQ(float expected, float result, float epsilon, float absError) {
  float absExpected = fabs(expected);
  float absResult = fabs(result);
  float diff = fabs(expected - result);


  if ((isnan(expected) == 1) && isnan(expected) == isnan(result))
    return true;

  if (expected == result) 
    return true;
  
  if (diff <= absError)
    return true;

  float largest = (absExpected > absResult) ? absExpected : absResult;

  if (diff <= largest * epsilon)
    return true;
  std::cout << "Expected: " << expected << " Got: " << result << std::endl;
  return false;
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);

  // invoke the test.
  return RUN_ALL_TESTS();
}
