#include "Message.hpp"

Message::Message(const Client &sender) : content(sender.get_buffer()), sender(sender) {};
