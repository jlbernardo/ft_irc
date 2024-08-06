2. The IRC Specification

2.1 Overview

   The protocol as described herein is for use both with server to
   server and client to server connections.  There are, however, more
   restrictions on client connections (which are considered to be
   untrustworthy) than on server connections.

2.2 Character codes

   No specific character set is specified. The protocol is based on a a
   set of codes which are composed of eight (8) bits, making up an
   octet.  Each message may be composed of any number of these octets;
   however, some octet values are used for control codes which act as
   message delimiters.

   Regardless of being an 8-bit protocol, the delimiters and keywords
   are such that protocol is mostly usable from USASCII terminal and a
   telnet connection.

   Because of IRC's scandanavian origin, the characters {}| are
   considered to be the lower case equivalents of the characters []\,
   respectively. This is a critical issue when determining the
   equivalence of two nicknames.

2.3 Messages

   Servers and clients send eachother messages which may or may not
   generate a reply.  If the message contains a valid command, as
   described in later sections, the client should expect a reply as
   specified but it is not advised to wait forever for the reply; client
   to server and server to server communication is essentially
   asynchronous in nature.

   Each IRC message may consist of up to three main parts: the prefix
   (optional), the command, and the command parameters (of which there
   may be up to 15).  The prefix, command, and all parameters are
   separated by one (or more) ASCII space character(s) (0x20).

   The presence of a prefix is indicated with a single leading ASCII
   colon character (':', 0x3b), which must be the first character of the
   message itself.  There must be no gap (whitespace) between the colon
   and the prefix.  The prefix is used by servers to indicate the true



Oikarinen & Reed                                                [Page 7]


RFC 1459              Internet Relay Chat Protocol              May 1993


   origin of the message.  If the prefix is missing from the message, it
   is assumed to have originated from the connection from which it was
   received.  Clients should not use prefix when sending a message from
   themselves; if they use a prefix, the only valid prefix is the
   registered nickname associated with the client.  If the source
   identified by the prefix cannot be found from the server's internal
   database, or if the source is registered from a different link than
   from which the message arrived, the server must ignore the message
   silently.

   The command must either be a valid IRC command or a three (3) digit
   number represented in ASCII text.

   IRC messages are always lines of characters terminated with a CR-LF
   (Carriage Return - Line Feed) pair, and these messages shall not
   exceed 512 characters in length, counting all characters including
   the trailing CR-LF. Thus, there are 510 characters maximum allowed
   for the command and its parameters.  There is no provision for
   continuation message lines.  See section 7 for more details about
   current implementations.

2.3.1 Message format in 'pseudo' BNF

   The protocol messages must be extracted from the contiguous stream of
   octets.  The current solution is to designate two characters, CR and
   LF, as message separators.   Empty  messages  are  silently  ignored,
   which permits  use  of  the  sequence  CR-LF  between  messages
   without extra problems.

   The extracted message is parsed into the components <prefix>,
   <command> and list of parameters matched either by <middle> or
   <trailing> components.

   The BNF representation for this is:


<message>  ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>
<prefix>   ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]
<command>  ::= <letter> { <letter> } | <number> <number> <number>
<SPACE>    ::= ' ' { ' ' }
<params>   ::= <SPACE> [ ':' <trailing> | <middle> <params> ]

<middle>   ::= <Any *non-empty* sequence of octets not including SPACE
               or NUL or CR or LF, the first of which may not be ':'>
<trailing> ::= <Any, possibly *empty*, sequence of octets not including
                 NUL or CR or LF>

<crlf>     ::= CR LF



Oikarinen & Reed                                                [Page 8]


RFC 1459              Internet Relay Chat Protocol              May 1993


NOTES:

  1)    <SPACE> is consists only of SPACE character(s) (0x20).
        Specially notice that TABULATION, and all other control
        characters are considered NON-WHITE-SPACE.

  2)    After extracting the parameter list, all parameters are equal,
        whether matched by <middle> or <trailing>. <Trailing> is just
        a syntactic trick to allow SPACE within parameter.

  3)    The fact that CR and LF cannot appear in parameter strings is
        just artifact of the message framing. This might change later.

  4)    The NUL character is not special in message framing, and
        basically could end up inside a parameter, but as it would
        cause extra complexities in normal C string handling. Therefore
        NUL is not allowed within messages.

  5)    The last parameter may be an empty string.

  6)    Use of the extended prefix (['!' <user> ] ['@' <host> ]) must
        not be used in server to server communications and is only
        intended for server to client messages in order to provide
        clients with more useful information about who a message is
        from without the need for additional queries.

   Most protocol messages specify additional semantics and syntax for
   the extracted parameter strings dictated by their position in the
   list.  For example, many server commands will assume that the first
   parameter after the command is the list of targets, which can be
   described with:

   <target>     ::= <to> [ "," <target> ]
   <to>         ::= <channel> | <user> '@' <servername> | <nick> | <mask>
   <channel>    ::= ('#' | '&') <chstring>
   <servername> ::= <host>
   <host>       ::= see RFC 952 [DNS:4] for details on allowed hostnames
   <nick>       ::= <letter> { <letter> | <number> | <special> }
   <mask>       ::= ('#' | '$') <chstring>
   <chstring>   ::= <any 8bit code except SPACE, BELL, NUL, CR, LF and
                     comma (',')>

   Other parameter syntaxes are:

   <user>       ::= <nonwhite> { <nonwhite> }
   <letter>     ::= 'a' ... 'z' | 'A' ... 'Z'
   <number>     ::= '0' ... '9'
   <special>    ::= '-' | '[' | ']' | '\' | '`' | '^' | '{' | '}'



Oikarinen & Reed                                                [Page 9]


RFC 1459              Internet Relay Chat Protocol              May 1993


   <nonwhite>   ::= <any 8bit code except SPACE (0x20), NUL (0x0), CR
                     (0xd), and LF (0xa)>

2.4 Numeric replies

   Most of the messages sent to the server generate a reply of some
   sort.  The most common reply is the numeric reply, used for both
   errors and normal replies.  The numeric reply must be sent as one
   message consisting of the sender prefix, the three digit numeric, and
   the target of the reply.  A numeric reply is not allowed to originate
   from a client; any such messages received by a server are silently
   dropped. In all other respects, a numeric reply is just like a normal
   message, except that the keyword is made up of 3 numeric digits
   rather than a string of letters.  A list of different replies is
   supplied in section 6.


Channel Operators

   The channel operator (also referred to as a "chop" or "chanop") on a
   given channel is considered to 'own' that channel.  In recognition of
   this status, channel operators are endowed with certain powers which
   enable them to keep control and some sort of sanity in their channel.
   As an owner of a channel, a channel operator is not required to have
   reasons for their actions, although if their actions are generally
   antisocial or otherwise abusive, it might be reasonable to ask an IRC
   operator to intervene, or for the usersjust leave and go elsewhere
   and form their own channel.

   The commands which may only be used by channel operators are:

        MODE    - Change the channel's mode
        KICK    - Eject a client from the channel
        INVITE  - Invite a client to an invite-only channel (mode +i)
        TOPIC   - Change the channel topic in a mode +t channel




Oikarinen & Reed                                                [Page 6]


RFC 1459              Internet Relay Chat Protocol              May 1993


   A channel operator is identified by the '@' symbol next to their
   nickname whenever it is associated with a channel (ie replies to the
   NAMES, WHO and WHOIS commands).

Mode message

      Command: MODE

   The MODE command is a dual-purpose command in IRC.  It allows both
   usernames and channels to have their mode changed.  The rationale for
   this choice is that one day nicknames will be obsolete and the
   equivalent property will be the channel.

   When parsing MODE messages, it is recommended that the entire message
   be parsed first and then the changes which resulted then passed on.

4.2.3.1 Channel modes

   Parameters: <channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>]
               [<ban mask>]

   The MODE command is provided so that channel operators may change the
   characteristics of `their' channel.  It is also required that servers
   be able to change channel modes so that channel operators may be
   created.

   The various modes available for channels are as follows:

           o - give/take channel operator privileges;
           p - private channel flag;
           s - secret channel flag;
           i - invite-only channel flag;
           t - topic settable by channel operator only flag;
           n - no messages to channel from clients on the outside;
           m - moderated channel;
           l - set the user limit to channel;



Oikarinen & Reed                                               [Page 21]


RFC 1459              Internet Relay Chat Protocol              May 1993


           b - set a ban mask to keep users out;
           v - give/take the ability to speak on a moderated channel;
           k - set a channel key (password).

   When using the 'o' and 'b' options, a restriction on a total of three
   per mode command has been imposed.  That is, any combination of 'o'
   and

4.2.3.2 User modes

   Parameters: <nickname> {[+|-]|i|w|s|o}

   The user MODEs are typically changes which affect either how the
   client is seen by others or what 'extra' messages the client is sent.
   A user MODE command may only be accepted if both the sender of the
   message and the nickname given as a parameter are both the same.

   The available modes are as follows:

           i - marks a users as invisible;
           s - marks a user for receipt of server notices;
           w - user receives wallops;
           o - operator flag.

   Additional modes may be available later on.

   If a user attempts to make themselves an operator using the "+o"
   flag, the attempt should be ignored.  There is no restriction,
   however, on anyone `deopping' themselves (using "-o").  Numeric
   Replies:

           ERR_NEEDMOREPARAMS              RPL_CHANNELMODEIS
           ERR_CHANOPRIVSNEEDED            ERR_NOSUCHNICK
           ERR_NOTONCHANNEL                ERR_KEYSET
           RPL_BANLIST                     RPL_ENDOFBANLIST
           ERR_UNKNOWNMODE                 ERR_NOSUCHCHANNEL

           ERR_USERSDONTMATCH              RPL_UMODEIS
           ERR_UMODEUNKNOWNFLAG

   Examples:

           Use of Channel Modes:

MODE #Finnish +im               ; Makes #Finnish channel moderated and
                                'invite-only'.

MODE #Finnish +o Kilroy         ; Gives 'chanop' privileges to Kilroy on



Oikarinen & Reed                                               [Page 22]


RFC 1459              Internet Relay Chat Protocol              May 1993


                                channel #Finnish.

MODE #Finnish +v Wiz            ; Allow WiZ to speak on #Finnish.

MODE #Fins -s                   ; Removes 'secret' flag from channel
                                #Fins.

MODE #42 +k oulu                ; Set the channel key to "oulu".

MODE #eu-opers +l 10            ; Set the limit for the number of users
                                on channel to 10.

MODE &oulu +b                   ; list ban masks set for channel.

MODE &oulu +b *!*@*             ; prevent all users from joining.

MODE &oulu +b *!*@*.edu         ; prevent any user from a hostname
                                matching *.edu from joining.

        Use of user Modes:

:MODE WiZ -w                    ; turns reception of WALLOPS messages
                                off for WiZ.

:Angel MODE Angel +i            ; Message from Angel to make themselves
                                invisible.

MODE WiZ -o                     ; WiZ 'deopping' (removing operator
                                status).  The plain reverse of this
                                command ("MODE WiZ +o") must not be
                                allowed from users since would bypass
                                the OPER command.

Kick command

      Command: KICK
   Parameters: <channel> <user> [<comment>]

   The KICK command can be  used  to  forcibly  remove  a  user  from  a
   channel.   It  'kicks  them  out'  of the channel (forced PART).



Oikarinen & Reed                                               [Page 25]


RFC 1459              Internet Relay Chat Protocol              May 1993


   Only a channel operator may kick another user out of a  channel.
   Each  server that  receives  a KICK message checks that it is valid
   (ie the sender is actually a  channel  operator)  before  removing
   the  victim  from  the channel.

   Numeric Replies:

           ERR_NEEDMOREPARAMS              ERR_NOSUCHCHANNEL
           ERR_BADCHANMASK                 ERR_CHANOPRIVSNEEDED
           ERR_NOTONCHANNEL

   Examples:

KICK &Melbourne Matthew         ; Kick Matthew from &Melbourne

KICK #Finnish John :Speaking English
                                ; Kick John from #Finnish using
                                "Speaking English" as the reason
                                (comment).

:WiZ KICK #Finnish John         ; KICK message from WiZ to remove John
                                from channel #Finnish

NOTE:
     It is possible to extend the KICK command parameters to the
following:

<channel>{,<channel>} <user>{,<user>} [<comment>]


Invite message

      Command: INVITE
   Parameters: <nickname> <channel>

   The INVITE message is used to invite users to a channel.  The
   parameter <nickname> is the nickname of the person to be invited to
   the target channel <channel>.  There is no requirement that the
   channel the target user is being invited to must exist or be a valid
   channel.  To invite a user to a channel which is invite only (MODE
   +i), the client sending the invite must be recognised as being a
   channel operator on the given channel.

   Numeric Replies:

           ERR_NEEDMOREPARAMS              ERR_NOSUCHNICK
           ERR_NOTONCHANNEL                ERR_USERONCHANNEL
           ERR_CHANOPRIVSNEEDED
           RPL_INVITING                    RPL_AWAY

   Examples:

   :Angel INVITE Wiz #Dust         ; User Angel inviting WiZ to channel
                                   #Dust

   INVITE Wiz #Twilight_Zone       ; Command to invite WiZ to
                                   #Twilight_zone


Topic message

      Command: TOPIC
   Parameters: <channel> [<topic>]

   The TOPIC message is used to change or view the topic of a channel.
   The topic for channel <channel> is returned if there is no <topic>
   given.  If the <topic> parameter is present, the topic for that
   channel will be changed, if the channel modes permit this action.

   Numeric Replies:

           ERR_NEEDMOREPARAMS              ERR_NOTONCHANNEL
           RPL_NOTOPIC                     RPL_TOPIC
           ERR_CHANOPRIVSNEEDED



Oikarinen & Reed                                               [Page 23]


RFC 1459              Internet Relay Chat Protocol              May 1993


   Examples:

   :Wiz TOPIC #test :New topic     ;User Wiz setting the topic.

   TOPIC #test :another topic      ;set the topic on #test to "another
                                   topic".

   TOPIC #test                     ; check the topic for #test.
