#include <iostream>
#include <memory>
#include <string>
#include "pipeline.cpp"

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
public:
  SimulationServiceStreamImpl(Queue<DataFrame*>* queue) : queue(queue) {
    // Create a data repo object
    std::unique_ptr<DataRepo> data_repo = std::make_unique<DataRepo>();

    // Set extract strategy to "list"
    data_repo->setExtractionStrategy("list");

    this->data_repo = *data_repo;
  }

  Status ReportCycle(ServerContext* context, const logdataanalyticsWithTime* request, Empty* reply) override {
    std::cout << "Received report at timestamp: " << request->timestamp() << std::endl;
    
    // Create a vector of strings from the request
    std::vector<std::string> data;
    for (const auto& log_entry : request->log()) {
      // Store the log entry in the vector (removing the '\n' character at the end of the string)
      data.push_back(log_entry.substr(0, log_entry.size() - 1));
    } 

    // Convert vector of strings to a dataframe
    DataFrame* df = data_repo.extractData("", ';', 0, data);

    // Set the timestamp of the dataframe
    df->setTimestamp(request->timestamp());

    // Add dataframe to the queue
    queue->push(df);

    return Status::OK;
  }

private:
  DataRepo data_repo;
  Queue<DataFrame*>* queue;
};

void RunServer(int num_threads, int max_queue_size) {
  // Create a queue of dataframes
  Queue<DataFrame*> queue(max_queue_size);
  
  std::string server_address("0.0.0.0:50051");
  SimulationServiceStreamImpl service(&queue);

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Process the pipeline
  process(&queue, max_queue_size, num_threads);

  server->Wait();
}

int main(int argc, char** argv) {
  int NUM_THREADS = 8;
  int MAX_QUEUE_SIZE = 20;

  RunServer(NUM_THREADS, MAX_QUEUE_SIZE);
  return 0;
}

// $ protoc -I ../../protos --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ../../protos/route_guide.proto
// $ protoc -I ../../protos --cpp_out=. ../../protos/route_guide.proto