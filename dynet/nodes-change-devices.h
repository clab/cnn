#ifndef DYNET_NODES_CHANGE_DEVICES_H
#define DYNET_NODES_CHANGE_DEVICES_H

#include "dynet/dynet.h"
#include "dynet/nodes-macros.h"

namespace dynet {

struct ToDevice : public Node {
  explicit ToDevice(const std::initializer_list<VariableIndex>& a, Device *device) : Node(a, device) {}
  virtual bool supports_multibatch() const override { return true; }
  DYNET_NODE_DEFINE_DEV_IMPL();
};

} // namespace dynet

#endif
