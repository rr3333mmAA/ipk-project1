
# IPK Calculator Protocol (IPKCPC)

## Description

IPKCPC is a C program that acts as a client to send requests to a server over TCP or UDP protocols. The program receives the server's IP address, port number, and protocol mode as command-line arguments.

The client sets up a connection with the server and sends requests in a loop until the user interrupts it. The client closes the connection with the server after finishing the loop.

## Additional information

The program consists of several functions that perform different tasks. The `setup_connection` function creates a socket and connects it to the server. The `send_request` function sends a request to the server, and the `recv_response` function receives a response from the server.


## Usage/Examples

**Compile**
```makefile
make
```

**Usage**
```bash
ipkcpc -h <host> -p <port> -m <mode>
```
where `host` is the IPv4 address of the server, `port` the server port, and `mode` either tcp or udp (e.g., `ipkcpc -h 1.2.3.4 -p 2023 -m udp`).

**Examples**
![1 example](http://url/to/img.png)
![2 example](http://url/to/img.png)
![3 example](http://url/to/img.png)
![4 example](http://url/to/img.png)


## License

[Mozilla Public License Version 2.0](https://choosealicense.com/licenses/mpl-2.0/)


## Bibliography

 - [TCP client reference](https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs\cpp\DemoTcp\client.c)
 - [UDP client reference](https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs\cpp\DemoUdp\client.c)
 - [Setting timeout](https://stackoverflow.com/questions/16163260/setting-timeout-for-recv-fcn-of-a-udp-socket)
 - [Socket programming](https://www.youtube.com/watch?v=LtXEMwSG5-8)
