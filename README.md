# Multithreaded Server 

EPITA - Week5 - Programming

![Workflow](https://github.com/ProtossDragoon/epita-w5-prog-multithreaded-server/actions/workflows/master.yaml/badge.svg)

## Quickstart

`terminal`
```bash
$ git clone https://github.com/ProtossDragoon/epita-w5-prog-multithreaded-server.git
$ cd epita-w5-prog-multithreaded-server
$ make
```

### shared_queue

Implementing simple circular linked list.

```bash
$ ./shared_queue
0: Pushes 83
1: Pushes 86
2: Pushes 77
0: Pops 83
0: Pushes 35
1: Pops 86
1: Pushes 92
3: Pushes 15
2: Pops 77
2: Pushes 49

[...]

7: Pushes 67
7: Pops 67
7: Pushes 29
7: Pops 29
```

### server

Implementing the Producer-Consumer Model. The shared queue must be accessed by multiple running threads.

```bash
$ ./server
```

Default port is 2048, default number of threads is 4.

`terminal1`
```bash
$ nc localhost 2048 4
hello
hello
```

`terminal2`
```bash
$ nc localhost 2048 4
world
world
```

`terminal3`
```bash
$ nc localhost 2048 4
hello
hello
world
world
```

`terminal4`
```bash
$ nc localhost 2048 4
a
a
```

`terminal5`
```bash
$ nc localhost 2048 4
b
```

`terminal3`
```bash
$ nc localhost 2048 4
hello
hello
world
world
^C
```

`terminal5`
```bash
$ nc localhost 2048 4
b
b
```

## Notes

If you are using VSCode, change your "type" in your launch.json file, e.g. `"type": "gdb"`
