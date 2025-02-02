#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstdlib>
#include <iostream>
#include <map>
#include "Client.hpp"

#include <string>

class Channel {
	
	private:

		std::string         _name;          // Channel name (starts with # or &)
		std::string         _topic;         // Channel topic
		std::string         _key;           // Channel password (if mode +k is set)
		std::map<int, Client*> _members;    // Channel members (fd -> Client*)
		std::map<int, Client*> _operators;  // Channel operators (fd -> Client*)
		
		// Channel modes
		bool _inviteOnly;      // mode +i
		bool _topicRestricted; // mode +t
		bool _hasKey;          // mode +k
		size_t _userLimit;     // mode +l

	public:	

		Channel(const std::string& name, Client* creator);
		~Channel();

		// Basic operations
		bool addMember(Client* client);
		bool removeMember(Client* client);
		void broadcastMessage(const std::string& message, Client* sender = NULL);
		
		// Channel operator operations
		void setOperator(Client* client);
		bool isOperator(Client* client) const;
		
		// Mode operations
		//if only operators are able to set modes, a setter function to do it
		// is not necessary, checkMode though, is necessary, because other entities
		// outside the Channel class may neeed to access this information   
		// void setMode(char mode, bool status);
		bool checkChannelModes(char mode) const;
		bool checkUserModes(char mode, Client* client);
		
		// Getters
		const std::string& getName() const;
		const std::string& getTopic() const;
		const std::map<int, Client*>& getMembers() const;
			
		// Required by subject
		bool kickMember(Client* operator_client, Client* target, const std::string& reason);
		bool inviteMember(Client* operator_client, Client* target);
		bool setTopic(Client* client, const std::string& new_topic);
};