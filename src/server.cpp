#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "data_analytics.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using data_analytics_package::Empty;
using data_analytics_package::logdataanalyticsWithTime;
using data_analytics_package::SimulationServiceStream;

class SimulationServiceStreamImpl final : public SimulationServiceStream::Service {
  Status ReportCycle(ServerContext* context, const logdataanalyticsWithTime* request, Empty* reply) override {
    std::cout << "Received report at timestamp: " << request->timestamp() << std::endl;
    for (const auto& log_entry : request->log()) {
      std::cout << "Log entry: " << log_entry << std::endl;
    }
    return Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  SimulationServiceStreamImpl service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();
  return 0;
}

// $ protoc -I ../../protos --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ../../protos/route_guide.proto
// $ protoc -I ../../protos --cpp_out=. ../../protos/route_guide.proto