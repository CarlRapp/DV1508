TotemPoleSystem = System()
TotemPoleSystem.MapCenterX = 0
TotemPoleSystem.MapCenterZ = 0
TotemPoleSystem.TotemPoles			=	{}
TotemPoleSystem.TotemPoles.__mode	=	"k"

TotemPoleSystem.ATAN2 = function(self, X, Z)
	if X > 0 then
		return math.atan(Z / X)
	elseif Z >= 0 and X < 0 then
		return math.atan(Z / X) + math.pi
	elseif Z < 0 and X < 0 then
		return math.atan(Z / X) - math.pi
	elseif Z > 0 and X == 0 then
		return math.pi / 2
	elseif Z < 0 and X == 0 then
		return -math.pi / 2
	else
		print("Undefined atan2 value (x == 0 and y == 0)")
	end
	
	return 0
end

TotemPoleSystem.Initialize = function(self)
	--	Set Name
	self:SetName("TotemPoleSystem")
	
	--	Toggle EntitiesAdded
	self:UsingEntitiesAdded()
	
	--	Set Filter
	self:AddComponentTypeToFilter("AddTotemPiece", FilterType.RequiresOneOf)
	self:AddComponentTypeToFilter("TotemPole", FilterType.RequiresOneOf)
	self:AddComponentTypeToFilter("Finishpoint", FilterType.RequiresOneOf)
	self:AddComponentTypeToFilter("Checkpoint", FilterType.RequiresOneOf)
	self:AddComponentTypeToFilter("MapSpecs", FilterType.RequiresOneOf)
end

TotemPoleSystem.AddFireplace = function(self, totemPoleId)
	local newFireplace	= 	world:CreateNewEntity()
	world:CreateComponentAndAddTo("Position", newFireplace)
	world:CreateComponentAndAddTo("Rotation", newFireplace)
	world:CreateComponentAndAddTo("Scale", newFireplace)
	world:CreateComponentAndAddTo("Model", newFireplace)
	world:CreateComponentAndAddTo("Color", newFireplace)
	world:CreateComponentAndAddTo("SyncNetwork", newFireplace)
	
	
	
	local rotation 		= 	world:GetComponent(newFireplace, "Rotation", 0)
	local position 		= 	world:GetComponent(newFireplace, "Position", 0)
	local scale			= 	world:GetComponent(newFireplace, "Scale", 0)
	world:SetComponent(newFireplace, "Model", "ModelName", "checkpointbonfireholder")
	world:SetComponent(newFireplace, "Model", "ModelPath", "checkpointbonfire")
	world:SetComponent(newFireplace, "Model", "RenderType", 0)
	
	scale:SetFloat3(0.6, 0.6, 0.6)
	
	local	X, Y, Z	=	world:GetComponent(totemPoleId, "Position", "X"):GetFloat3()
	local tempMoveX = self.MapCenterX - X
	local tempMoveZ = self.MapCenterZ - Z
	-- Position
	local offsetX = 0.35;
	local offsetZ = 0.35;
	
	if tempMoveX < 0 then
		offsetX = offsetX * -1
	else
		offsetX = offsetX * 1
	end
	
	if tempMoveZ < 0 then
		offsetZ = offsetZ * -1
	else
		offsetZ = offsetZ * 1
	end
	
	position:SetFloat3(X + offsetX, 0.58, Z + offsetZ)
end

TotemPoleSystem.AddTopPiece = function(self, totemPoleId, R, G, B)
	local	totemId	=	self:AddTotemPiece(0, totemPoleId, R, G, B)
	world:SetComponent(totemId, "Model", "ModelName", "totemtop")
	world:SetComponent(totemId, "Model", "ModelPath", "totemtop")
	world:SetComponent(totemId, "Model", "RenderType", 0)
	
	self:AddFireplace(totemPoleId)
end

TotemPoleSystem.AddTotemPiece = function(self, currentPlayerNumber, totemPoleId, R, G, B)

	local totemPiece	= world:CreateNewEntity("TotemPiece")
	local rotation 		= world:GetComponent(totemPiece, "Rotation", 0)
	local position 		= world:GetComponent(totemPiece, "Position", 0)
	local scale			= world:GetComponent(totemPiece, "Scale", 0)
	local tpHeight		=	world:GetComponent(totemPiece, "TotemPiece", "CurrentHeight")
	local tpPoleId		=	world:GetComponent(totemPiece, "TotemPiece", "TotemPoleId")
	tpHeight:SetInt(0)
	tpPoleId:SetInt(totemPoleId)

	world:CreateComponentAndAddTo("Color", totemPiece)
	local color			= 	world:GetComponent(totemPiece, "Color", "X")
	color:SetFloat3(R, G, B)
	
	local	X, Y, Z	=	world:GetComponent(totemPoleId, "Position", "X"):GetFloat3()
	
	local tempMoveX = self.MapCenterX - X
	local tempMoveZ = self.MapCenterZ - Z

	local	totemAngle	=	math.pi/2 - self:ATAN2(tempMoveX, tempMoveZ)
	
	--local	test	=	math.atan2(tempMoveX, tempMoveZ)
	
	
	-- Rotation
	rotation:SetFloat3(0, totemAngle, 0)
	-- Position
	local offsetX = 0.35;
	local offsetZ = 0.35;
	
	if tempMoveX > 0 then
		offsetX = offsetX * -1
	else
		offsetX = offsetX * 1
	end
	
	if tempMoveZ > 0 then
		offsetZ = offsetZ * -1
	else
		offsetZ = offsetZ * 1
	end
	
	
	

	position:SetFloat3(X + offsetX, 0.68, Z + offsetZ)
	-- Scale
	scale:SetFloat3(1,1,1)
	world:SetComponent(totemPiece, "Model", "ModelName", "totem")
	world:SetComponent(totemPiece, "Model", "ModelPath", "totem")
	world:SetComponent(totemPiece, "Model", "RenderType", 0)
	
	return totemPiece
end

TotemPoleSystem.CreateTotemPole = function(self, totemCheckpointNumber, tX, tZ)
	--	Create the first totempole if none was found 
	local newTotemPole = world:CreateNewEntity()
	world:CreateComponentAndAddTo("TotemPole", newTotemPole)
	world:CreateComponentAndAddTo("CheckpointId", newTotemPole)
	
	world:SetComponent(newTotemPole, "TotemPole", "Height", 0)
	world:GetComponent(newTotemPole, "Position", "X"):SetFloat3(tX, 1, tZ)
	world:SetComponent(newTotemPole, "CheckpointId", "Id", totemCheckpointNumber)
	
	self.TotemPoles[#self.TotemPoles+1]	=	newTotemPole
	return newTotemPole
end

TotemPoleSystem.CheckCheckPoints = function(self, targetCpId, totemId, playerNum, R, G, B)

	local checkPoints = self:GetEntities("Checkpoint")
	for i = 1, #checkPoints do
	
		local cpId = world:GetComponent(checkPoints[i], "Checkpoint", "Number"):GetInt()
		
		-- Found the correct checkpoint
		if cpId == targetCpId then
		
			-- Get the checkPoints' position
			local cpPosX, cpPosZ = world:GetComponent(checkPoints[i], "MapPosition", 0):GetInt2()
			-- Get the height of the current totempole
			local height = world:GetComponent(totemId, "TotemPole", "Height"):GetInt()
			-- Add a new piece
			self.AddTotemPiece(self, playerNum, totemId, R, G, B)
			world:SetComponent(totemId, "TotemPole", "Height", height + 1)
			return
		end
		
	end
end

TotemPoleSystem.CheckAddTotemPiece = function(self, entityId)

	--	Get Player and Checkpoint number
	local	playerNumber		=	world:GetComponent(entityId, "PlayerNumber", "Number"):GetInt()
	local	targetCheckpointId	=	world:GetComponent(entityId, "CheckpointId", "Id"):GetInt()
	local	R, G, B				=	world:GetComponent(entityId, "Color", "X"):GetFloat3(0)
	
	--	Get the actual totem pole (if there is one)
	local	totemPoles	=	self:GetEntities("TotemPole")
	local	totemPoleId	=	-1
	
	for iTotem = 1, #totemPoles do
		local	totemCheckpointId	=	world:GetComponent(totemPoles[iTotem], "CheckpointId", 0):GetInt()
		if totemCheckpointId == targetCheckpointId then
			totemPoleId = totemPoles[iTotem]
			break
		end
	end
	
	if totemPoleId == -1 then
		return
	end
	
	self:CheckCheckPoints(targetCheckpointId, totemPoleId, playerNumber, R, G, B)
end

TotemPoleSystem.EntitiesAdded = function(self, dt, addedEntities)

	for n = 1, #addedEntities do
		--	Get entity ID
		local newEntity = addedEntities[n]
		
		--	Add Totem Piece logic
		if world:EntityHasComponent(newEntity, "AddTotemPiece") then
			self:CheckAddTotemPiece(newEntity)
		end
		
		--	New Checkpoint is added
		if world:EntityHasComponent(newEntity, "Checkpoint") then
			local tX, tZ = world:GetComponent(newEntity, "MapPosition", 0):GetInt2()
			local checkpointId = world:GetComponent(newEntity, "Checkpoint", "Number"):GetInt()
			local newTotempoleId	=	self:CreateTotemPole(checkpointId, tX, tZ)
			
		end
		
		--	Get Center and also set up all angles on TotemPoles
		if world:EntityHasComponent(newEntity, "MapSpecs") then
			local tX, tZ = world:GetComponent(newEntity, "MapSpecs", "SizeX"):GetInt2()
			
			self.MapCenterX = tX * 0.5
			self.MapCenterZ = tZ * 0.5
			
			for i = 1, #self.TotemPoles do
				self:AddTopPiece(self.TotemPoles[i], 0.0, 0.0, 0.0)
			end
		end
	
	
	end 
end