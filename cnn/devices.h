#ifndef CNN_DEVICES_H
#define CNN_DEVICES_H

#include <string>
#include "cnn/aligned-mem-pool.h"
#include "cnn/cuda.h"

namespace Eigen {
  struct DefaultDevice;
  class CudaStreamDevice;
  struct GpuDevice;
}

namespace cnn {

enum class DeviceType {CPU, GPU};

struct ComputationGraph; // TODO is there a nicer way to resolve this cyclic dependency?
struct DeviceMemCheckpoint;

class Device {
 protected:
  Device(DeviceType t, MemAllocator* m) : type(t), mem(m) {}
  Device(const Device&) = delete;
  Device& operator=(const Device&) = delete;
  virtual ~Device();
 public:
  DeviceType type;
  MemAllocator* mem;
  AlignedMemoryPool* fxs;
  AlignedMemoryPool* dEdfs;
  AlignedMemoryPool* ps;
  float* kSCALAR_MINUSONE;
  float* kSCALAR_ONE;
  float* kSCALAR_ZERO;
  std::string name;
  virtual DeviceMemCheckpoint mark(ComputationGraph *cg);
  virtual void revert(DeviceMemCheckpoint cp);
};

#if HAVE_CUDA
class Device_GPU : public Device {
 public:
  typedef Eigen::CudaStreamDevice EigenDevice;
  explicit Device_GPU(int mb, int device_id);
  ~Device_GPU();
  int cuda_device_id;
  cublasHandle_t cublas_handle;
  Eigen::GpuDevice* edevice;
  Eigen::CudaStreamDevice* estream;
  GPUAllocator gpu_mem;
};
#endif

class Device_CPU : public Device {
 public:
  typedef Eigen::DefaultDevice EigenDevice;
  explicit Device_CPU(int mb, bool shared);
  ~Device_CPU();
  CPUAllocator cpu_mem;
  Eigen::DefaultDevice* edevice;
  MemAllocator* shmem;
};

} // namespace cnn

#endif
