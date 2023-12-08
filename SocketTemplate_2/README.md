#### SocketTemplate_2 is an improved version of SocketTemplate_1

## SYNOPSIS
- Command Line Argument integration added
- Deprecated functions upgraded to new versions
	- `inet_ntoa` -> `inet_ntop`
	- `inet_addr` and `gethostbyname` -> `getaddrinfo`

## DESCRIPTION

It is a good practice to retrieve the server and port numbers relevant to the client and server programs using command-line arguments when developing client and server programs. For instance, client program can be run as follows:

    ./client -s <server_address> -p <server_port> -b <source_port>

The -s, -p, and -b options have default values. If these are not specified at the command line, their default values will be used.

Similarly, the server program should be run as shown below:

    ./server -p <port_number>

The port number here represents the port that the server listens on. This parameter also have a default value.

---

Over time, more capable functions have been developed to replace some classic socket functions. The older functions were designed for IPv4. With IPv6, more general versions of these functions have been created to support both IPv4 and IPv6.

One well-known function, **inet_ntoa**, used to convert a 4-byte IPv4 address into dotted-decimal notation. This function has an advanced form, known as **inet_ntop**, which encompasses IPv6 as well.

Another new function introduced with IPv6 is **getaddrinfo**. This function is essentially an extended version of **inet_addr** and **gethostbyname**, accommodating IPv6 as well. **getaddrinfo** not only converts numeric addresses to dotted-decimal format but also, if a non dotted-decimal format is provided (which might occur if a server is specified with a hostname like *www.google.com*), it performs a DNS query to obtain the IP address of the relevant host.
