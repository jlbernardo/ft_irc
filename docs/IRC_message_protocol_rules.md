## Understanding the IRC Specification for Messages and Commands

To build a solid C++ IRC server, we must understand how the IRC clients will format and send messages. This is the key to handling messages correctly and building features that work as expected.

**Core Concepts:**

* **Client-Server Architecture:** IRC uses a client-server model, where clients connect to servers to communicate with each other.
* **Messages:** Communication happens through messages, which are lines of text terminated by a CR-LF (Carriage Return - Line Feed) pair.
* **Message Format:** Messages consist of three main parts:
    * **Prefix (Optional):** Indicates the origin of the message, preceded by a colon (`:`).
    * **Command:** A word or three-digit number representing the action to be performed.
    * **Parameters (Optional):**  Provide additional information for the command, separated by spaces.
* **Channels:** Virtual rooms where users can gather and chat, denoted by a `#` or `&` prefix.
* **Channel Operators (Chanops):** Users with special privileges within a channel, denoted by an `@` symbol next to their nickname.

**Key Commands:**

* **MODE:** Used to change the settings (modes) of a channel or user. For channels, commonly used modes include:
    * `+i`: Invite-only mode, requiring an invitation to join.
    * `+t`: Topic-restricted mode, allowing only chanops to change the topic.
    * `+k`: Key mode, requiring a password to join.
    * `+o`: Grants channel operator privileges to a user.
    * `+l`: Sets a user limit for the channel.
* **KICK:** Removes a user from a channel. Only chanops can use this command.
* **INVITE:** Invites a user to a channel. For invite-only channels, only chanops can use this command.
* **TOPIC:**  Sets or displays the topic of a channel. In topic-restricted channels, only chanops can set the topic.

**Example Messages:**

* **Joining a Channel:** `JOIN #MyChannel`
* **Sending a Message:** `PRIVMSG #MyChannel :Hello everyone!`
* **Kicking a User:** `KICK #MyChannel User1 :Reason for kicking`
* **Inviting a User:** `INVITE User2 #MyChannel`
* **Setting a Topic:** `TOPIC #MyChannel :This is the new topic`
* **Setting a Channel Key (Password):** `MODE #MyChannel +k MySecretPassword`
* **Setting Invite-Only Mode:** `MODE #MyChannel +i`
* **Setting Topic-Restricted Mode:** `MODE #MyChannel +t`
* **Granting Channel Operator Privileges:** `MODE #MyChannel +o User3`
* **Setting a User Limit:** `MODE #MyChannel +l 25`


**Numeric Replies:**

Servers often respond to commands with numeric replies, indicating success or failure. These replies are three-digit numbers and can be found in the IRC specification documentation.
For example, a 200 code usually means everything went smoothly, while a 403 code might indicate that the client tried to access a channel they weren't authorized to join.
