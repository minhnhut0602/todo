#!/bin/sh

[ -e scripts ] || { echo 'Execute this script from root dir.'; exit 1; }

TAR_FILE=target/todo-linux-bench-x64.tar.gz
ZIP_FILE=target/todo-win-bench-x64.zip

COMMON_FILES='ARGS.txt PORT.txt todo-all/target/todo-all-jarjar.jar todo-ts/g/user/UserServices.json bench/README.md bench/run.properties bench/run.sh bench/run-uri.sh bench/payload'

echo "========== tar.gz"
rm -f $TAR_FILE
tar -cvzf $TAR_FILE target/protostuffdb-bench target/hprotostuffdb-bench target/hprotostuffdb-rmaster-bench  $COMMON_FILES

echo "\n========== zip"
rm -f $ZIP_FILE
zip -r $ZIP_FILE target/protostuffdb-bench.exe $COMMON_FILES
