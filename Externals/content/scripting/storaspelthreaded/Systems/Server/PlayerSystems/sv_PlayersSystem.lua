PlayersSystem = System()
PlayersSystem.NextSlot = 1
PlayersSystem.FreeSlots = {}

PlayersSystem.Initialize = function(self)
	self:SetName("PlayersConnectedSystem")
	
	--	Toggle EntitiesAdded
	self:UsingEntitiesAdded()
	self:UsingEntitiesRemoved()
	
	self:AddComponentTypeToFilter("Player", FilterType.Mandatory)
	self:AddComponentTypeToFilter("ActiveNetConnection", FilterType.Mandatory)
	self:AddComponentTypeToFilter("IsSpectator", FilterType.Excluded)
end

PlayersSystem.EntitiesAdded = function(self, dt, taskIndex, taskCount, entities)

	for n = 1, #entities do
		local entityId = entities[n]
	
		local playerNumber
		if #self.FreeSlots ~= 0 then
			playerNumber = self.FreeSlots[1]
			table.remove(self.FreeSlots, 1)
		else
			playerNumber = self.NextSlot
			self.NextSlot = self.NextSlot + 1
		end

		local newEntityId = world:CreateNewEntity("Unit")

		world:SetComponent(newEntityId, "Model", "ModelName", "ply" .. playerNumber);
		world:SetComponent(newEntityId, "Model", "ModelPath", "caveman");
		world:SetComponent(newEntityId, "Model", "RenderType", 0);

		world:SetComponent(newEntityId, "PlayerNumber", "Number", playerNumber)
		world:SetComponent(newEntityId, "PlayerEntityId", "Id", entityId)
		world:SetComponent(newEntityId, "TargetCheckpoint", "Id", 1)
		world:GetComponent(newEntityId, "Direction", 0):SetInt2(0, -1)
		world:CreateComponentAndAddTo("NeedSpawnLocation", newEntityId)


		world:SetComponent(entityId, "PlayerNumber", "Number", playerNumber)
		world:SetComponent(entityId, "UnitEntityId", "Id", newEntityId)

		local ip = world:GetComponent(entityId, "NetConnection", "IpAddress"):GetString()
		local port = world:GetComponent(entityId, "NetConnection", "Port"):GetInt()


		print("Unit ", newEntityId)

		local id = Net.StartPack("Client.SendPlayerUnitId")
		Net.WriteInt(id, playerNumber)
		Net.Send(id, ip, port)
	end
end

PlayersSystem.EntitiesRemoved = function(self, dt, taskIndex, taskCount, entities)

	for n = 1, #entities do
		local entityId = entities[n]

		local plyNum = self:GetComponent(entityId, "PlayerNumber", "Number"):GetInt()
		table.insert(self.FreeSlots, plyNum)

		local unitId = self:GetComponent(entityId, "UnitEntityId", "Id"):GetInt()

		world:KillEntity(unitId)
	end
end