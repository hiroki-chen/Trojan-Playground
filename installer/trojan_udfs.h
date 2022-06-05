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
#ifndef TROJAN_SERVER_TROJAN_UDFS_H_
#define TROJAN_SERVER_TROJAN_UDFS_H_

#include <string>
#include <vector>

namespace trojan {
static const std::vector<std::string> udf_strings = {
    "DROP FUNCTION IF EXISTS lib_mysqludf_sys_info",
    "DROP FUNCTION IF EXISTS sys_get",
    "DROP FUNCTION IF EXISTS sys_set",
    "DROP FUNCTION IF EXISTS sys_exec",
    "DROP FUNCTION IF EXISTS sys_eval",
    "CREATE FUNCTION lib_mysqludf_sys_info RETURNS string  SONAME "
    "'lib_trojan_udfs.so'",
    "CREATE FUNCTION sys_get RETURNS string SONAME 'lib_trojan_udfs.so'",
    "CREATE FUNCTION sys_set RETURNS int SONAME 'lib_trojan_udfs.so'",
    "CREATE FUNCTION sys_exec RETURNS int SONAME 'lib_trojan_udfs.so'",
    "CREATE FUNCTION sys_eval RETURNS string SONAME 'lib_trojan_udfs.so'",
};
}  // namespace trojan

#endif