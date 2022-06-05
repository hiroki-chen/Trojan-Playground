/*
 Copyright (c) 2022 Haobin Chen

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef TROJAN_SERVER_TROJAN_SERVER_H_
#define TROJAN_SERVER_TROJAN_SERVER_H_

#include <string>

#include <grpc++/grpc++.h>
#include <spdlog/spdlog.h>

#include "protos/trojan_messages.pb.h"
#include "protos/trojan_messages.grpc.pb.h"

extern std::shared_ptr<spdlog::logger> logger;

namespace trojan {
class TrojanService final : public TrojanBaseService::Service {
 public:
  grpc::Status PrintStringOnServer(grpc::ServerContext* context,
                                   const SimpleStringMessage* request,
                                   google::protobuf::Empty* response) override;

  grpc::Status ShutDownServer(grpc::ServerContext* context,
                              const ShutDownServerMessage* message,
                              google::protobuf::Empty* response) override;

  grpc::Status ShutDownCancel(grpc::ServerContext* context,
                              const google::protobuf::Empty* message,
                              google::protobuf::Empty* response) override;

  grpc::Status GetFileList(grpc::ServerContext* context,
                           const SimpleStringMessage* message,
                           SimpleStringMessage* response) override;

  grpc::Status GetSnapShot(grpc::ServerContext* context,
                           const google::protobuf::Empty* message,
                           SimpleStringMessage* response) override;

  grpc::Status DeleteFile(grpc::ServerContext* context,
                          const SimpleStringMessage* message,
                          google::protobuf::Empty* response) override;

  grpc::Status UploadFile(grpc::ServerContext* context,
                          const FileMessage* message,
                          google::protobuf::Empty* response) override;

  grpc::Status DownloadFile(grpc::ServerContext* context,
                            const SimpleStringMessage* message,
                            FileMessage* response) override;
};

class ServerRunner final {
 public:
  ~ServerRunner() {}

  static std::unique_ptr<ServerRunner> GetInstance();
  void Run(void);

 private:
  ServerRunner(const std::string& address, const std::string& port)
      : address_(address), port_(port) {} 

  std::string address_;
  std::string port_;
  std::unique_ptr<TrojanService> service_;
  std::unique_ptr<grpc::Server> server_;
};
}  // namespace trojan

#endif