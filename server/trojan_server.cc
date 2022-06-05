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
#include "trojan_server.h"

#include <thread>
#include <chrono>
#include <filesystem>
#include <fstream>

#include <unistd.h>
#include <linux/reboot.h>
#include <sys/syscall.h>

#include <absl/flags/declare.h>
#include <absl/flags/flag.h>

ABSL_DECLARE_FLAG(std::string, trojan_server_address);
ABSL_DECLARE_FLAG(std::string, trojan_server_port);

namespace trojan {
grpc::Status TrojanService::PrintStringOnServer(
    grpc::ServerContext* context, const SimpleStringMessage* message,
    google::protobuf::Empty* response) {
  // Print the string on server side.
  const std::string str = message->message();
  logger->info("From peer {}, got message: {}", context->peer(), str);

  return grpc::Status::OK;
}

grpc::Status TrojanService::ShutDownServer(grpc::ServerContext* context,
                                           const ShutDownServerMessage* message,
                                           google::protobuf::Empty* response) {
  // Shut down the server.
  logger->info("The client {} wants to shut down the server.", context->peer());

  if (getuid() != 0) {
    logger->error("The server is not root.");
    return grpc::Status(grpc::StatusCode::PERMISSION_DENIED,
                        "Permission denied.");
  }

  const uint32_t count_down = message->count_down();
  logger->info("The server will shut down in {} seconds.", count_down);

  // Sleep for a while.
  std::this_thread::sleep_for(std::chrono::seconds(count_down));

  // Shut down the server.
  logger->info("The server is shutting down.");
  syscall(SYS_reboot, LINUX_REBOOT_CMD_POWER_OFF, 0, 0, 0, 0);

  return grpc::Status::OK;
}

grpc::Status TrojanService::ShutDownCancel(
    grpc::ServerContext* context, const google::protobuf::Empty* message,
    google::protobuf::Empty* response) {
  // Cancel the shut down.
  logger->info("The client {} wants to cancel the shut down.", context->peer());

  if (getuid() != 0) {
    logger->error("The server is not root.");
    return grpc::Status(grpc::StatusCode::PERMISSION_DENIED,
                        "Permission denied.");
  }

  logger->info("The server is canceling the shut down.");
  syscall(SYS_reboot, LINUX_REBOOT_CMD_CAD_ON, 0, 0, 0, 0);

  return grpc::Status::OK;
}

grpc::Status TrojanService::GetFileList(grpc::ServerContext* context,
                                        const SimpleStringMessage* message,
                                        SimpleStringMessage* response) {
  logger->info("The client {} wants to get the file list.", context->peer());

  const std::string directory = message->message();
  // Check if the directory is valid.
  if (!std::filesystem::exists(directory)) {
    logger->error("The directory {} does not exist.", directory);
    return grpc::Status(grpc::StatusCode::NOT_FOUND, "Directory not found.");
  }

  // Get the file list.
  std::string file_list = "";
  std::string file_name;

  for (const auto& entry : std::filesystem::directory_iterator(directory)) {
    file_name = entry.path().filename().string();
    if (file_name == "." || file_name == "..") {
      continue;
    }
    file_list += file_name + "\n";
  }

  // Set the response.
  response->set_message(file_list);

  return grpc::Status::OK;
}

grpc::Status TrojanService::GetSnapShot(grpc::ServerContext* context,
                                        const google::protobuf::Empty* message,
                                        SimpleStringMessage* response) {
  logger->info("The client {} wants to get the snapshot.", context->peer());

  if (getenv("DISPLAY") == nullptr) {
    const std::string error =
        "The server is not running in a graphical environment.";
    logger->error(error);
    return grpc::Status(grpc::StatusCode::NOT_FOUND, error);
  }

  return grpc::Status::OK;
}

grpc::Status TrojanService::DeleteFile(grpc::ServerContext* context,
                                       const SimpleStringMessage* message,
                                       google::protobuf::Empty* response) {
  logger->info("The client {} wants to delete the file {}.", context->peer(),
               message->message());

  const std::string file_name = message->message();
  // Check if the file is valid.
  if (!std::filesystem::exists(file_name)) {
    logger->error("The file {} does not exist.", file_name);
    return grpc::Status(grpc::StatusCode::NOT_FOUND, "File not found.");
  }

  // Check if we have the permission to delete the file.
  if (std::filesystem::perms::none ==
      (std::filesystem::status(file_name).permissions() &
       std::filesystem::perms::owner_write)) {
    logger->error("The file {} does not have the permission to delete.",
                  file_name);
    return grpc::Status(grpc::StatusCode::PERMISSION_DENIED,
                        "Permission denied.");
  }

  // Delete the file.
  std::filesystem::remove(file_name);

  return grpc::Status::OK;
}

grpc::Status TrojanService::UploadFile(grpc::ServerContext* context,
                                       const FileMessage* message,
                                       google::protobuf::Empty* response) {
  logger->info("The client {} wants to upload the file {}.", context->peer(),
               message->name());

  const std::string file_name = message->name();
  // Check if the file is valid.
  if (std::filesystem::exists(file_name)) {
    logger->error("The file {} already exists.", file_name);
    return grpc::Status(grpc::StatusCode::ALREADY_EXISTS,
                        "File already exists.");
  }

  // Write the file.
  std::ofstream file(file_name, std::ios::out | std::ios::binary);
  file.write(message->content().c_str(), message->content().size());
  file.close();

  return grpc::Status::OK;
}

grpc::Status TrojanService::DownloadFile(grpc::ServerContext* context,
                                         const SimpleStringMessage* message,
                                         FileMessage* response) {
  logger->info("The client {} wants to download the file {}.", context->peer(),
               message->message());

  const std::string file_name = message->message();
  // Check if the file is valid.
  if (!std::filesystem::exists(file_name)) {
    logger->error("The file {} does not exist.", file_name);
    return grpc::Status(grpc::StatusCode::NOT_FOUND, "File not found.");
  }

  // Read the file.
  std::ifstream file(file_name, std::ios::in | std::ios::binary);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  file.close();

  // Set the response.
  response->set_name(file_name);
  response->set_content(content);

  return grpc::Status::OK;
}

std::unique_ptr<ServerRunner> ServerRunner::GetInstance() {
  static std::unique_ptr<ServerRunner> instance(
      new ServerRunner(absl::GetFlag(FLAGS_trojan_server_address),
                       absl::GetFlag(FLAGS_trojan_server_port)));
  return std::move(instance);
}

void ServerRunner::Run(void) {
  service_ = std::make_unique<TrojanService>();
  // Start the server.
  grpc::ServerBuilder builder;
  const std::string addr = address_ + ":" + port_;
  builder.AddListeningPort(addr, grpc::InsecureServerCredentials());
  builder.RegisterService(service_.get());

  server_ = builder.BuildAndStart();
  logger->info("The server is running on {}:{}.", address_, port_);

  // Wait for the server to shutdown.
  server_->Wait();
}
}  // namespace trojan