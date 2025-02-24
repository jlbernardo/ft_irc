#pragma once
#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "ft_irc.hpp"

class Channel {
	private:
		std::string         	_name;       // Channel name (starts with # or &)
		std::string         	_topic;      // Channel topic
		std::string         	_key;        // Channel pass (if mode +k is set)
		std::deque<int> 		_join_order; // Members join order
		std::map<int, Client*>	_members;    // Channel members (fd -> Client*)
		std::map<int, Client*>	_invited;    // Channel invited members
		std::map<int, Client*>	_operators;  // Channel operators (fd -> Client*)
		
		// Channel modes
		bool _inviteOnly;      // mode +i
		bool _topicRestricted; // mode +t
		bool _hasKey;          // mode +k
		size_t _userLimit;     // mode +l

	public:
		~Channel();
		Channel(const Channel &copy);
		Channel& operator=(const Channel &copy);
		Channel(const std::string& name, Client* creator);

		// Basic operations
		void addMember(Client* client);
		void removeMember(Client* client);
		void broadcast(Client* sender = NULL, const std::string& message = "");
		
		// Channel operator operations
		void addOperator(Client* client);
		void removeOperator(Client* client);
		void promoteFirstMember();
		bool isOperator(Client* client) const;
		bool isInvited(Client* client) const;
		int getCurrentMembersCount();
		int getCurrentOperatorsCount();
		int getUserLimit();
		std::string getModes() const;
		std::string getModeParams() const;
		
		void setInviteOnly(bool value);
		void setTopicRestricted(bool value);
		void setKey(const std::string& key);
		void setUserLimit(size_t limit);
		
		bool mode(char mode) const;
		bool checkUserModes(char mode, Client* client);
		bool isMember(Client* client) const;
		
		// Getters
		const std::string& getName() const;
		const std::string& getTopic() const;
		const std::string& getKey() const;
		const std::map<int, Client*>& getMembers() const;
			
		// Required by subject
		void inviteMember(Client* target);
		void consumeInvite(Client* target);
		void setTopic(const std::string& new_topic);

		//validation
		bool parseChannelName(const std::string& name) const;
};

#endif
