var fs = require('fs'),
    path = require('path'),
    os = require('os'),
    win32 = os.platform() === 'win32',
    start_str = 'jni rpc: ',
    pdb_started = false,
    pdb,
    wnd,
    rpc_host

function println(str) {
    process.stdout.write(str + '\n')
}

function getChildArgs(initial_args, extra_args, child_cwd) {
    return initial_args.concat(extra_args, [
        '-Djava.class.path=' + path.join(child_cwd, 'todo-all/target/todo-all-jarjar.jar'),
        'todo.all.Main',
        child_cwd
    ])
}

function isDir(dir) {
    return fs.statSync(dir).isDirectory()
}

function findSubDir(baseDir, subDirPrefix) {
    var dirs = fs.readdirSync(baseDir), dir, f
    for (var i = 0, len = dirs.length; i < len; i++) {
        if (isDir(dir=path.join(baseDir, f=dirs[i])) && 0 === f.indexOf(subDirPrefix))
            return dir
    }
    return null
}

function resolveBin(child_cwd) {
    if (win32) return path.join(child_cwd, 'target/protostuffdb.exe')
    
    var bin = path.join(child_cwd, 'target/protostuffdb-rjre')
    return fs.existsSync(bin) && fs.existsSync(path.join(child_cwd, 'target/jre')) ?
            bin : path.join(child_cwd, 'target/protostuffdb')
}

function startProtostuffdb() {
    var spawn = require('child_process').spawn,
        child_cwd = path.join(__dirname, '..'),
        bin = resolveBin(child_cwd),
        port = fs.readFileSync(path.join(child_cwd, 'PORT.txt'), 'utf8').trim(),
        extra_args = fs.readFileSync(path.join(child_cwd, 'ARGS.txt'), 'utf8').trim().split(' '),
        child_args = getChildArgs(['127.0.0.1:' + port, path.join(__dirname, 'g/user/UserServices.json')], extra_args, child_cwd),
        target_cwd,
        p

    if (!win32) {
        target_cwd = child_cwd
    } else if (isDir(target_cwd = 'C:/opt/jre/bin/server') ||
            isDir(target_cwd = 'C:/Program Files/Java/jdk1.7.0_79/jre/bin/server')) {
    } else if (fs.existsSync(p = path.join(child_cwd, 'JDK_DIR.txt'))) {
        if (!isDir(target_cwd = path.join(fs.readFileSync(p, 'utf8').trim(), 'jre/bin/server'))) {
            println('JDK_DIR.txt did not contain a valid jdk path.')
            process.exit(1)
            return
        }
    } else if (!isDir('C:/Program Files/Java') ||
            !(p = findSubDir('C:/Program Files/Java', 'jdk1.7')) ||
            !isDir(target_cwd = path.join(p, 'jre/bin/server'))) {
        println('Please create JDK_DIR.txt in the root dir of the project that contains the full path to your jdk.')
        println('E.g.\nC:/path/to/jdk1.7.0_79')
        process.exit(1)
        return
    }

    pdb = spawn(bin, child_args, { cwd: target_cwd })
    pdb.on('error', onChildError)
    pdb.stdout.on('data', onChildOut)
    pdb.on('close', onChildClose)
    rpc_host = 'http://127.0.0.1:' + port
}

function onChildError(err) {
    println('protostuffdb spawn failed')
    process.exit(1)
}

function isStart(data) {
    return data.length > start_str.length && start_str === String(data.slice(0, start_str.length))
}

function onChildOut(data) {
    if (pdb_started || !isStart(data))
        return
    pdb_started = true
    openWindow()
}

function onChildClose(code) {
    if (code)
        println('child process exited with code ' + code)
    if (wnd)
        wnd.close(true)
    pdb = undefined
}

function onClose() {
    this.hide()
    wnd = undefined
    if (pdb)
        pdb.kill()
    this.close(true)
}

function onOpen(w) {
    wnd = w
    w.resizeTo(430, 715)
    w.moveTo(430, 0)
    if (pdb)
        w.on('close', onClose)
    w.show()
}

function openWindow() {
    global.rpc_host = rpc_host
    nw.Window.open('index.html', { show: false, show_in_taskbar: !win32 }, onOpen)
}

startProtostuffdb()

