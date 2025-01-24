**Basic checks**


- There is a Makefile, the project compiles correctly with the required options, is written in C++, and the executable is called as expected.
- Ask and check how many poll() (or equivalent) are present in the code. There must be only one.
- Verify that the poll() (or equivalent) is called every time before each accept, read/recv, write/send. After these calls, errno should not be used to -    trigger specific action (e.g. like reading again after errno == EAGAIN).
- Verify that each call to fcntl() is done as follows: fcntl(fd, F_SETFL, O_NONBLOCK); Any other use of fcntl() is forbidden.
- If any of these points is wrong, the evaluation ends now and the final mark is 0.


**Networking**

Check the following requirements:

- The server starts, and listens on all network interfaces on the port given from the command line.
- Using the 'nc' tool, you can connect to the server, send commands, and the server answers you back.
- Ask the team what is their reference IRC client.
- Using this IRC client, you can connect to the server.
- The server can handle multiple connections at the same time. The server should not block. It should be able to answer all demands. Do some test with the IRC client and nc at the same time.
- Join a channel thanks to the appropriate command. Ensure that all messages from one client on that channel are sent to all other clients that joined the channel.


**Networking specials**

Network communications can be disturbed by many strange situations.

- Just like in the subject, using nc, try to send partial commands. Check that the server answers correctly. With a partial command sent, ensure that other connections still run fine.
- Unexpectedly kill a client. Then check that the server is still operational for the other connections and for any new incoming client.
- Unexpectedly kill a nc with just half of a command sent. Check again that the server is not in an odd state or blocked.
- Stop a client (^-Z) connected on a channel. Then flood the channel using another client. The server should not hang. When the client is live again, all stored commands should be processed normally. Also, check for memory leaks during this operation.


**Client Commands basic**

- With both nc and the reference IRC client, check that you can authenticate, set a nickname, a username, join a channel. This should be fine (you should have already done this previously).
- Verify that private messages (PRIVMSG) are fully functional with different parameters.


**Client Commands channel operator**

- With both nc and the reference IRC client, check that a regular user does not have privileges to do channel operator actions. Then test with an operator. All the channel operation commands should be tested (remove one point for each feature that is not working).
