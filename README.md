# full-stack todo app

A full-stack todo app with:
- c++ backend (leveldb + uWebSockets) with java (jni) stored procs
- dart (flutter) mobile frontend (todo-dart)
- typescript (vuets) web frontend (todo-ts)
- c++ (nana) desktop frontend (todo-cpp)

Desktop-client downloads:
 - [linux64@1M](https://unpkg.com/pdb-todo@0.5.1/bin/todo-desktop-client-linux-x64.tar.gz)
 - [win64@1.7M](https://unpkg.com/pdb-todo@0.5.1/bin/todo-desktop-client-win-x64.zip)

By default it connects to https://todo.dyuproject.com (see target/ARGS.txt)

Desktop-standalone (client+server+jre) downloads:
 - [linux64@13.9M](https://unpkg.com/pdb-todo@0.5.0/bin/todo-desktop-standalone-linux-x64.tar.gz)
 - [win64@18M](https://unpkg.com/pdb-todo@0.5.0/bin/todo-desktop-standalone-win-x64.zip)

![screenshot](https://github.com/dyu/bookmarks/raw/master/screenie-lubuntu.png)
![screenshot](https://github.com/dyu/bookmarks/raw/master/screenie-ubuntu.png)
![screenshot](https://github.com/dyu/bookmarks/raw/master/screenie-win8.png)

The web app is available [here](https://apps.dyuproject.com/todo/)

The mobile app is available [here](https://play.google.com/store/apps/details?id=com.dyuproject.todo)

The web and mobile apps have the same semantics.
Swipe horizontally to nagivate the pages (paginate).
Long-press the item to update.

## Server runtime dependencies
- jdk7

## Desktop runtime dependencies
- jdk7

## Desktop dev requirements (c++)
- [ninja](https://ninja-build.org/)
- [gn](https://refi64.com/gn-builds/)

## Web dev requirements
- [node](https://nodejs.org/en/download/) 6.9.0 or higher
- [yarn](https://yarnpkg.com/en/)

## Common dev requirements
- jdk7 (at /usr/lib/jvm/java-7-oracle)
- [maven](https://maven.apache.org/download.cgi)
- [protostuffdb](https://gitlab.com/dyu/protostuffdb) (downloaded below)

## Setup
```sh
mkdir -p target/data/main
echo "Your data lives in user/ dir.  Feel free to back it up." > target/data/main/README.txt

# download protostuffdb
yarn add protostuffdb@0.19.0 && mv node_modules/protostuffdb/dist/* target/ && rm -f package.json yarn.lock && rm -r node_modules

wget -O target/fbsgen-ds.jar https://repo1.maven.org/maven2/com/dyuproject/fbsgen/ds/fbsgen-ds-fatjar/1.0.17/fbsgen-ds-fatjar-1.0.17.jar
./modules/codegen.sh
mvn install
```

### To build the c++ desktop frontend, visit todo-cpp/README.md

### To build the dart mobile frontend, visit todo-dart/README.md

## Web dev setup
```sh
cd todo-ts
yarn install

# build css
yarn run ns.b
```

## Dev mode
```sh
# produces a single jar the first time (todo-all/target/todo-all-jarjar.jar)
./run.sh

# on another terminal
cd todo-ts
# serves the ui via http://localhost:8080/
yarn run dev
```

## Production mode
If ```run.sh``` is still running, stop that process (ctrl+c)
```sh
cd todo-ts
# produces a single js and other assets in todo-ts/dist/
yarn run build
# finally, run your production app
nw .
# or
node chrome-app.js
```

