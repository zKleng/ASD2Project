#include "MemoryManager.h"
#include "memory_service.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <iostream>

class MemoryServiceImpl final : public MemoryService::Service {
private:
    MemoryManager memoryManager;

public:
    MemoryServiceImpl(size_t size, const std::string& dumpFolder) : memoryManager(size, dumpFolder) {}

    grpc::Status Create(grpc::ServerContext* context, const CreateRequest* request, CreateResponse* response) override {
        if (request->size() <= 0) {
            return grpc::Status(grpc::INVALID_ARGUMENT, "El tamaño debe ser mayor a 0.");
        }

        int id = memoryManager.create(request->size());
        if (id == -1) {
            return grpc::Status(grpc::RESOURCE_EXHAUSTED, "No hay suficiente memoria disponible.");
        }

        response->set_id(id);
        return grpc::Status::OK;
    }

    grpc::Status Set(grpc::ServerContext* context, const SetRequest* request, SetResponse* response) override {
        if (request->id() < 0) {
            return grpc::Status(grpc::INVALID_ARGUMENT, "ID inválido.");
        }

        bool success = memoryManager.set(request->id(), request->value().data(), request->value().size());
        response->set_success(success);
        return success ? grpc::Status::OK : grpc::Status(grpc::NOT_FOUND, "Bloque no encontrado.");
    }

    grpc::Status Get(grpc::ServerContext* context, const GetRequest* request, GetResponse* response) override {
        if (request->id() < 0) {
            return grpc::Status(grpc::INVALID_ARGUMENT, "ID inválido.");
        }

        std::string value = memoryManager.get(request->id());
        if (value.empty()) {
            return grpc::Status(grpc::NOT_FOUND, "Bloque no encontrado.");
        }

        response->set_value(value);
        return grpc::Status::OK;
    }

    grpc::Status IncreaseRef(grpc::ServerContext* context, const RefRequest* request, RefResponse* response) override {
        memoryManager.increaseRef(request->id());
        response->set_success(true);
        return grpc::Status::OK;
    }

    grpc::Status DecreaseRef(grpc::ServerContext* context, const RefRequest* request, RefResponse* response) override {
        memoryManager.decreaseRef(request->id());
        response->set_success(true);
        return grpc::Status::OK;
    }
};

int main(int argc, char* argv[]) {
    std::string server_address = "0.0.0.0:50051";
    MemoryServiceImpl service(1024 * 1024, "./dumps");

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

    std::cout << "Servidor en ejecución en " << server_address << std::endl;
    server->Wait();
}
