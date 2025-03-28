#include <iostream>
#include <memory>
#include <string>
#include "memory_service.grpc.pb.h"
#include <grpcpp/grpcpp.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using memoryservice::MemoryService;
using memoryservice::CreateRequest;
using memoryservice::CreateResponse;
using memoryservice::SetRequest;
using memoryservice::SetResponse;
using memoryservice::GetRequest;
using memoryservice::GetResponse;
using memoryservice::RefRequest;
using memoryservice::RefResponse;

class MemoryManagerClient {
public:
    MemoryManagerClient(std::shared_ptr<Channel> channel)
        : stub_(MemoryService::NewStub(channel)) {}

    int createMemory(int size, const std::string& type) {
        CreateRequest request;
        request.set_size(size);
        request.set_type(type);  // Se agrega el tipo

        CreateResponse response;
        ClientContext context;

        Status status = stub_->Create(&context, request, &response);
        if (status.ok()) {
            return response.id();
        } else {
            std::cerr << "Error en Create: " << status.error_message() << std::endl;
            return -1;
        }
    }

    bool setMemory(int id, const std::string& value) {
        SetRequest request;
        request.set_id(id);
        request.set_value(value);

        SetResponse response;
        ClientContext context;

        Status status = stub_->Set(&context, request, &response);
        if (!status.ok()) {
            std::cerr << "Error en Set: " << status.error_message() << std::endl;
        }
        return status.ok() && response.success();
    }

    std::string getMemory(int id) {
        GetRequest request;
        request.set_id(id);

        GetResponse response;
        ClientContext context;

        Status status = stub_->Get(&context, request, &response);
        if (status.ok()) {
            return response.value();
        } else {
            std::cerr << "Error en Get: " << status.error_message() << std::endl;
            return "";
        }
    }

    bool increaseRef(int id) {
        RefRequest request;
        request.set_id(id);

        RefResponse response;
        ClientContext context;

        Status status = stub_->IncreaseRef(&context, request, &response);
        return status.ok() && response.success();
    }

    bool decreaseRef(int id) {
        RefRequest request;
        request.set_id(id);

        RefResponse response;
        ClientContext context;

        Status status = stub_->DecreaseRef(&context, request, &response);
        return status.ok() && response.success();
    }

private:
    std::unique_ptr<MemoryService::Stub> stub_;
};

int main(int argc, char** argv) {
    MemoryManagerClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    int id = client.createMemory(128, "int");
    if (id != -1) {
        std::cout << "Bloque creado con ID: " << id << std::endl;
        client.setMemory(id, "Hola, Memoria!");
        std::cout << "Valor almacenado: " << client.getMemory(id) << std::endl;
        client.increaseRef(id);
        client.decreaseRef(id);
    }

    return 0;
}

