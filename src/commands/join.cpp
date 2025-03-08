#include "ft_irc.hpp"


void chanKeySplitter(std::string raw, char delimiter, std::vector<std::string>& chanKeyVec)
{
	std::stringstream ss(raw);
	std::string word;

	while (!ss.eof()) {
		getline(ss, word, delimiter);
		chanKeyVec.push_back(word);
	}
}

void parseJoinInput(const std::string& input, std::vector<std::string>& channelsVec, std::vector<std::string>& keysVec) {
    size_t chanKeyDelimiter = input.find(" ");
	std::string channels;
	std::string keys;
    
    if (chanKeyDelimiter == std::string::npos) {
        channels = input;
        keys = "";
    } else {
        channels = input.substr(0, chanKeyDelimiter);
        
        std::string remainingPart = input.substr(chanKeyDelimiter + 1);
        
        size_t secondSpacePos = remainingPart.find(" ");
        
        if (secondSpacePos == std::string::npos) {
            keys = remainingPart;
        } else {
            keys = remainingPart.substr(0, secondSpacePos);
        }
    }

	chanKeySplitter(channels, ',', channelsVec);
	chanKeySplitter(keys, ',', keysVec);
}

void join(Commands &commands, Command &cmd) {
	Channel* channel;
	Client &sender = commands.get_sender();
	Server &server = sender.getServer();
	std::vector<std::string> channels;
	std::vector<std::string> keys;
	parseJoinInput(cmd.params, channels, keys);
	std::string channel_name;

	for (size_t i = 0; i < channels.size(); i++) {
		channel_name = channels[i];

		if (server.channelExists(channel_name)) {
			channel = server.get_channels()[channel_name];
	
			if (channel->getMembers().find(sender.get_fd()) != channel->getMembers().end()) {
				server.send_message(sender.get_fd(), ERR_USERONCHANNEL(sender.get_nickname(), channel_name));
				return ;
			}
			if (channel->mode('l') && channel->getCurrentMembersCount() >= channel->getUserLimit()) {
				server.send_message(sender.get_fd(), ERR_CHANNELISFULL(channel_name));
				return ;
			}
			if (channel->mode('i') && !channel->isInvited(&sender)) {
				server.send_message(sender.get_fd(), ERR_INVITEONLYCHAN(channel_name));
				return ;
			}
			if (channel->mode('k') && i < keys.size()) {
				if (channel->getKey() != keys[i]) {
					server.send_message(sender.get_fd(), ERR_BADCHANNELKEY(sender.get_username(), channel_name));
					return ;
				} 
			}
			channel->addMember(&sender);
		}
		else {
			Channel* created_channel = new Channel(channel_name, &sender);
			server.addChannel(created_channel);
			channel = created_channel;
	
			if (server.get_channels().find(channel_name) == server.get_channels().end()) {
				logger.warn("Failed to create channel: " + channel_name);
				return ;
			}
	
			logger.info("Channel created successfully: " + channel_name);
		}
	
		sender.add_channel(channel);
	
		if (channel->mode('i'))
			channel->consumeInvite(&sender);
	
		for (std::map<int, Client *>::const_iterator it = channel->getMembers().begin(); it != channel->getMembers().end(); ++it) {
			Client* member = it->second;
			server.send_message(member->get_fd(), RPL_JOIN(sender.get_identifier(), channel_name));
		}
	
		if (!channel->getTopic().empty())
			server.send_message(sender.get_fd(), RPL_TOPIC(sender.get_nickname(), channel_name, channel->getTopic()));
	
		for (std::map<int, Client *>::const_iterator it = channel->getMembers().begin(); it != channel->getMembers().end(); ++it) {
			Client* member = it->second;
			std::string prefix = "";
	
			if (channel->isOperator(member))
				prefix = "@";
	
			server.send_message(sender.get_fd(), RPL_NAMREPLY(member->get_nickname(), channel_name, prefix + member->get_nickname()));
		}
	
		server.send_message(sender.get_fd(), RPL_ENDOFNAMES(sender.get_nickname(), channel_name)); 
	}
}