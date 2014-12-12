networkMessagesSystem = System()

networkMessagesSystem.Update = function(self, dt)

	if Server.IsRunning() then
		Server.Update(dt)
	end
	
	if Client.IsConnected() then
		Client.Update(dt)
	end
	
	if Server.IsRunning() then
		while Server.HandlePacket() > 0 do	end
	end
		
	if Client.IsConnected() then
		while Client.HandlePacket() > 0 do	end
	end

end
networkMessagesSystem.Initialize = function(self)
	self:InitializeNetworkEvents()

	self:AddComponentTypeToFilter("Network", FilterType.Mandatory)
	print("NetworkMessagesSystem initialized!")
end
networkMessagesSystem.OnEntityAdded = function(self, entityId)
	print("OnEntityAdded (LUA)")
end
networkMessagesSystem.OnEntityRemoved = function(self, entityId)
	print("OnEntityRemoved (LUA)")
end

networkMessagesSystem.OnBannedFromServer = function(self, _ip, _port, _message)
	local s = "[Client] You was banned from the server " .. _ip .. ":" .. _port .. ". Reason: " .. _message
	Console.Print(s)
end
networkMessagesSystem.OnConnectedToServer = function(self, _ip, _port)
	local s = "[Client] Connected to server " .. _ip .. ":" .. _port
	Console.Print(s)
end
networkMessagesSystem.OnDisconnectedFromServer = function(self, _ip, _port)
	local s = "[Client] Disconnected from server " .. _ip .. ":" .. _port
	Console.Print(s)
end
networkMessagesSystem.OnFailedToConnect = function(self, _ip, _port)
	local s = "[Client] Failed to connect to server " .. _ip .. ":" .. _port
	Console.Print(s)	
end
networkMessagesSystem.OnKickedFromServer = function(self, _ip, _port, _message)
	local s = "[Client] Kicked from server " .. _ip .. ":" .. _port .. ". Reason: " .. _message
	Console.Print(s)	
end

networkMessagesSystem.OnPasswordInvalid = function(self, _ip, _port)
	local s = "[Client] Invalid password to server " .. _ip .. ":" .. _port
	Console.Print(s)	
end
networkMessagesSystem.OnRemotePlayerBanned = function(self, _message)
	local s = "[Client] Remote player " .. _message .. " was banned from the server"
	Console.Print(s)
end
networkMessagesSystem.OnRemotePlayerConnected = function(self, _message)
	local s = "[Client] Remote player " .. _message .. " connected"
	Console.Print(s)
end
networkMessagesSystem.OnRemotePlayerDisconnected = function(self, _message)
	local s = "[Client] Remote player " .. _message .. " disconnected"
	Console.Print(s)
end
networkMessagesSystem.OnRemotePlayerKicked = function(self, _message)
	local s = "[Client] Remote player " .. _message .. " was kicked from the server"
	Console.Print(s)
end
networkMessagesSystem.OnServerFull = function(self, _ip, _port)
	local s = "[Client] Failed to connect to server " .. _ip .. ":" .. _port .. ". Server full"
	Console.Print(s)	
end
networkMessagesSystem.OnTimedOutFromServer = function(self, _ip, _port)
	local s = "[Client] Timed out from server " .. _ip .. ":" .. _port
	Console.Print(s)	
end

networkMessagesSystem.OnPlayerConnected = function(self, _ip, _port, _message)

	local s = "[Server] " .. _message .. " (" .. _ip .. ":" .. _port .. ") connected to server"

	
	Console.Print(s)
	
	local id = Client.StartPack("Test")
	Client.WriteFloat(id, 1.5)
	Client.WriteString(id, "String")
	Client.WriteBool(id, true)
	Client.WriteInt(id, 5)
	Client.Send(id)
	
	local id = Server.StartPack("Test2")
	Server.Send(id, _ip, _port)

end
networkMessagesSystem.OnPlayerDisconnected = function(self, _ip, _port, _message)
	local s = "[Server] " .. _message .. " (" .. _ip .. ":" .. _port .. ") disconnected from server"
	Console.Print(s)
end
networkMessagesSystem.OnPlayerTimedOut = function(self, _ip, _port, _message)
	local s = "[Server] " .. _message .. " (" .. _ip .. ":" .. _port .. ") timed out from the server"
	Console.Print(s)
end