# IRC and TCP/IP, what's the connection?

The [Internet Relay Chat (IRC)](https://www.amazon.com.br/Book-IRC-Ultimate-Guide-Internet/dp/1886411298) was implemented on top of the [Internet Protocol suite (commonly known as TCP/IP)](https://en.wikipedia.org/wiki/Internet_protocol_suite). This means that the IRC protocol uses TCP/IP to establish connections between clients and servers, and to send and receive messages.

<br>

>  *IRC has been implemented on top of TCP since TCP supplies a reliable
   network protocol which is well suited to this scale of conferencing.
   The use of multicast IP is an alternative, but it is not widely
   available or supported at the present time. — [J. Oikarinen](https://en.wikipedia.org/wiki/Jarkko_Oikarinen) and [D. Reed](https://datatracker.ietf.org/doc/html/rfc1324) on [RfC1459](https://datatracker.ietf.org/doc/html/rfc1459)*

<br>

So, because it implements TCP/IP, all messages are guaranteed to be delivered in the correct order, without duplicates, and without errors. This is important for a chat system, as it ensures that the messages are received by all participants in the same order they were sent.


The officially assigned ports are:

| Protocol | Port |
|----------|------|
| IRC | 194 |
| [IRC-Serv](https://www.adminsub.net/tcp-udp-port-finder/529) | 529 |
| [IRCS](https://annvix.com/secure_irc_authentication_and_encryption) | 994 |

<br>

However, as these ports are within the priviledged range (0-1024) for UNIX systems, they require that the IRC daemon ([IRCd](https://en.wikipedia.org/wiki/IRCd)) run with superuser priviledges. To avoid the security risks associated with the use of priviledged ports, most IRC servers use the range 6665-6669, [being 6667 the most common port](https://www.infosecinstitute.com/resources/incident-response-resources/internet-relay-chat-irc-protocol-with-wireshark/) for the protocol. It's good to know, though, that an IRC server can be configured to listen on other ports as well.

*Note: an IRC daemon is [any server application](https://www.linux.org/threads/tcp-ip-protocols-internet-relay-chat-irc.9386/) using the IRC protocol.*

<br>

## How does TCP/IP work?

<br>
<p align="center">
  <img src="https://linuxhint.com/wp-content/uploads/2021/05/image2-3.png"/>
</p>

As TCP/IP wishes to ensure a reliable communication between two devices, it uses a [three-way handshake](https://learn.microsoft.com/en-us/troubleshoot/windows-server/networking/three-way-handshake-via-tcpip) to establish a connection. This handshake is a process of three steps:

<br>

1. **SYN**: the client sends a packet with the SYN flag set to the server, asking to establish a connection.
2. **SYN-ACK**: the server responds with a packet that has both the SYN and ACK flags set, acknowledging the request and asking to establish a connection.
3. **ACK**: the client sends a packet with the ACK flag set, acknowledging the server's response.

<br>

After the handshake is complete, the connection is established and the devices can start sending data to each other. The [data is sent in packets](https://cs.stanford.edu/people/eroberts/courses/soco/projects/1999-00/internet/tcp.html), which are small units of data sent over the network. These packets are received by the other device, which sends an acknowledgment back to the sender and, if the sender doesn't receive an acknowledgment, it resends the packet.

<br>

### What does this mean for IRC?

After the connection is stablished, IRC messages are encapsulated within TCP/IP segments. While TCP/IP guarantees the delivery of these segments in the correct order, the messages are formatted according to the IRC protocol, specifying commands, parameters, and content.

TCP/IP also implements [error checks](https://www.prepbytes.com/blog/computer-network/error-control-in-tcp/) and [recovery mechanisms](https://www.researchgate.net/publication/4017906_On_the_performance_of_TCP_loss_recovery_mechanisms), retransmitting the packages that were lost or corrupted, and takes care of the message [flow control](https://www.baeldung.com/cs/tcp-flow-control-vs-congestion-control), preventing that one end overwhelms the other with too much data and adjusting the rate of data transmission based on the receiver's capacity.

To summarize, TCP/IP provides the foundation for IRC to function effectively by offering a reliable and ordered byte stream for transmitting chat messages between clients and servers.
