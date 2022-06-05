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
#ifndef TROJAN_INSTALLER_H_
#define TROJAN_INSTALLER_H_

#include <memory>
#include <string>

#include <cppconn/driver.h>
#include <cppconn/statement.h>
#include <mysql_connection.h>
#include <spdlog/logger.h>

namespace trojan {
extern std::shared_ptr<spdlog::logger> logger;

class Installer {
  sql::Driver* driver_;
  std::unique_ptr<sql::Connection> connection_;
  std::unique_ptr<sql::Statement> statement_;

  const std::string address_;
  const int port_;
  const std::string username_;
  const std::string passwd_;

  bool is_initialized_;

  void InitMySQL(void);

 public:
  Installer(const std::string& address, const int port,
            const std::string& username, const std::string& passwd);

  bool InstallTrojan(const std::string& file_path, const std::string& service_path);
};
}  // namespace trojan

#endif