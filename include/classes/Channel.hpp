#pragma once
#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "ft_irc.hpp"

class Channel {
	
	private:
		std::string         	_name;       // Channel name (starts with # or &)
		std::string         	_topic;      // Channel topic
		std::string         	_key;        // Channel pass (if mode +k is set)
		std::map<int, Client*>	_members;    // Channel members (fd -> Client*)
		std::map<int, Client*>	_operators;  // Channel operators (fd -> Client*)
		
		// Channel modes
		bool _inviteOnly;      // mode +i
		bool _topicRestricted; // mode +t
		bool _hasKey;          // mode +k
		size_t _userLimit;     // mode +l

	public:	

		Channel(const std::string& name, Client* creator);
		Channel(const Channel &copy);
		~Channel();

		// Basic operations
		bool addMember(Client* client);
		bool removeMember(Client* client);
		void broadcast(Client* sender = NULL, const std::string& message = "");
		
		// Channel operator operations
		void setOperator(Client* client);
		bool isOperator(Client* client) const;
		int getCurrentMembersCount();
		int getUserLimit();
		
		// Mode operations
		// if only operators are able to set modes, a setter function to do it
		// is not necessary, checkMode though, is necessary, because other entities
		// outside the Channel class may neeed to access this information   
		// void setMode(char mode, bool status);
		bool mode(char mode) const;
		bool checkUserModes(char mode, Client* client);
		bool isMember(Client* client) const;
		
		// Getters
		const std::string& getName() const;
		const std::string& getTopic() const;
		const std::map<int, Client*>& getMembers() const;
			
		// Required by subject
		bool kickMember(Client* operator_client, Client* target, const std::string& reason);
		bool inviteMember(Client* operator_client, Client* target);
		bool setTopic(Client* client, const std::string& new_topic);

		//validation
		bool parseChannelName(const std::string& name) const;
};

#endif
