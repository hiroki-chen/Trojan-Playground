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
#include "trojan_installer.h"

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

ABSL_FLAG(std::string, address, "localhost", "The address of the MySQL server");
ABSL_FLAG(int, port, 3306, "The port of the MySQL server");
ABSL_FLAG(std::string, username, "root", "The username of the MySQL server");
ABSL_FLAG(std::string, passwd, "123456", "The password of the MySQL server");
ABSL_FLAG(std::string, file_path, "./trojan", "The path of the Trojan binary");
ABSL_FLAG(std::string, service_path, "../service/trojan_server.service",
          "The path of the service file");

std::shared_ptr<spdlog::logger> trojan::logger =
    spdlog::stderr_color_mt("trojan_installer");

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);

  const std::string address = absl::GetFlag(FLAGS_address);
  const int port = absl::GetFlag(FLAGS_port);
  const std::string username = absl::GetFlag(FLAGS_username);
  const std::string passwd = absl::GetFlag(FLAGS_passwd);

  try {
    std::unique_ptr<trojan::Installer> installer =
        std::make_unique<trojan::Installer>(address, port, username, passwd);

    if (installer->InstallTrojan(absl::GetFlag(FLAGS_file_path),
                                 absl::GetFlag(FLAGS_service_path))) {
      trojan::logger->info("[+] Trojan installed successfully");
      exit(1);
    } else {
      trojan::logger->error("[-] Trojan installation failed");
    }
  } catch (const sql::SQLException& e) {
    trojan::logger->error("[-] {}: MySQL error: {}", e.getErrorCode(),
                          e.what());
  }
  return 0;
}