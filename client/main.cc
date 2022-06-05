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
#include <fstream>

#include <grpc++/grpc++.h>

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>

#include "protos/trojan_messages.grpc.pb.h"
#include "protos/trojan_messages.pb.h"

ABSL_FLAG(std::string, address, "localhost", "Address to listen on");
ABSL_FLAG(std::string, port, "1234", "Port to listen on");

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  // Create a stub.
  const std::string addr =
      absl::GetFlag(FLAGS_address) + ":" + absl::GetFlag(FLAGS_port);
  auto stub = trojan::TrojanBaseService::NewStub(
      grpc::CreateChannel(addr, grpc::InsecureChannelCredentials()));

  // Create a request.
  trojan::SimpleStringMessage message;
  google::protobuf::Empty empty;
  message.set_message("/usr/local/lib");
  {
    grpc::ClientContext context;

    trojan::SimpleStringMessage reponse;

    stub->GetFileList(&context, message, &reponse);
    std::cout << reponse.message() << std::endl;
  }

  {
    grpc::ClientContext context;
    stub->PrintStringOnServer(&context, message, &empty);
  }

  {
    grpc::ClientContext context;
    trojan::FileMessage message;
    message.set_name("./myFile.txt");

    // Read file from the txt.
    std::ifstream file("../myFile.txt");
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    message.set_content(content);

    stub->UploadFile(&context, message, &empty);
  }

  {
    grpc::ClientContext context;
    trojan::SimpleStringMessage message;
    message.set_message("./myFile.txt");
    
    stub->DeleteFile(&context, message, &empty);
  }

  {
    grpc::ClientContext context;
    trojan::SimpleStringMessage message;
    message.set_message("../CMakeCache.txt");
    trojan::FileMessage response;

    stub->DownloadFile(&context, message, &response);

    // Store to the file.
    std::ofstream file("./myFile.txt");
    file << response.content();
    file.close();

  }

  {
    grpc::ClientContext context;
    google::protobuf::Empty empty;
    trojan::ShutDownServerMessage message;
    message.set_count_down(60);
    stub->ShutDownServer(&context, message, &empty);

  }

  {
    grpc::ClientContext context;
    google::protobuf::Empty empty;
    stub->ShutDownCancel(&context, empty, &empty);
  }

  return 0;
}