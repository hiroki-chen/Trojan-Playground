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

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <absl/flags/flag.h>
#include <absl/flags/parse.h>

#include "server/trojan_server.h"

std::shared_ptr<spdlog::logger> logger =
    spdlog::stderr_color_mt("trojan_server");

ABSL_FLAG(std::string, trojan_server_address, "localhost", "Address to listen on");
ABSL_FLAG(std::string, trojan_server_port, "1234", "Port to listen on");

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);

  const std::unique_ptr<trojan::ServerRunner> server_runner =
      trojan::ServerRunner::GetInstance();
  server_runner->Run();
  return 0;
}
