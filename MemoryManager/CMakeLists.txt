cmake_minimum_required(VERSION 3.14)
project(MemoryManager)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encontrar las bibliotecas necesarias
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

# Directorios de código fuente y protobuf
set(PROTO_DIR ${CMAKE_SOURCE_DIR})
set(SRC_DIR ${CMAKE_SOURCE_DIR})

# Archivo .proto
set(PROTO_FILE ${PROTO_DIR}/memory_service.proto)

# Generación de archivos .pb.cc y .pb.h
protobuf_generate(
  LANGUAGE cpp
  PROTOS ${PROTO_FILE}
  OUT_VAR PROTO_SRCS
)

protobuf_generate(
  LANGUAGE cpp
  PROTOS ${PROTO_FILE}
  OUT_VAR GRPC_SRCS
  PLUGIN "protoc-gen-grpc=${GRPC_CPP_PLUGIN}"
  GENERATE_EXTENSIONS ".grpc.pb.h;.grpc.pb.cc"
)

# Lista de archivos fuente
set(SRC_FILES
    ${SRC_DIR}/Client.cpp
    ${SRC_DIR}/Server.cpp
    ${SRC_DIR}/MemoryManager.cpp
    ${PROTO_SRCS}
    ${GRPC_SRCS}
)

# Crear el ejecutable del servidor
add_executable(memory_manager_server ${SRC_FILES})
target_link_libraries(memory_manager_server
    gRPC::grpc++
    protobuf::libprotobuf
    pthread
)

# Crear el ejecutable del cliente
add_executable(memory_manager_client ${SRC_DIR}/Client.cpp ${PROTO_SRCS} ${GRPC_SRCS})
target_link_libraries(memory_manager_client
    gRPC::grpc++
    protobuf::libprotobuf
    pthread
)
include_directories(${CMAKE_BINARY_DIR})  # Para incluir archivos generados

# Mensaje de éxito
message(STATUS "✅ Configuración CMake completada. Para compilar, usa: `make`")
