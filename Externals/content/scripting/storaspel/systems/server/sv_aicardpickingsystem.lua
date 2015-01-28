AiCardPickingSystem = System()
AiCardPickingSystem.NumberOfCardsToPick = 5
AiCardPickingSystem.CardsPerHand = 8

AiCardPickingSystem.Initialize = function(self)
	self:SetName("AI card picking System")
	
	self:AddComponentTypeToFilter("AI", FilterType.RequiresOneOf)
	self:AddComponentTypeToFilter("AICard", FilterType.RequiresOneOf)
	self:AddComponentTypeToFilter("TileComp", FilterType.RequiresOneOf)
	
	
end

AiCardPickingSystem.Update = function(self, dt)
	
	local AIs = self:GetEntities("AI")
	local Cards = self:GetEntities("AICard")
	local CPtiles = self:GetEntities("Checkpoint")
	
	for i = 1, #AIs do
		
		local unitID = self:GetComponent(AIs[i], "UnitEntityId", 0):GetInt()
		
		local cpTargetNr = self:GetComponent(unitID, "TargetCheckpoint", 0):GetInt()
		
		-- vart AIn vill
		local targetPositionX, targetPositionY = self:GetTargetPosition(CPtiles, cpTargetNr)
		-- vart AIn är
		local aiPositonX, aiPositonY = self:GetComponent(unitID, "MapPosition", 0):GetInt2()
		-- AIs direction
		local aiDirX, aiDirY = self:GetComponent(unitID, "Direction", 0):GetInt2()
		--Fetch the cards which is relevant to the current AI
		local CardSetAI = self:GetAIsCardSet(AIs[i], Cards)
		--This will catch the best 
		local PickedCards = self:AIPickCards(CardSetAI, aiDirX, aiDirY, aiPositonX, aiPositonY, targetPositionX, targetPositionY)

		if #PickedCards >= self.NumberOfCardsToPick then	
			
			self:SendCards(PickedCards, AIs[i])
		end
	end
end

AiCardPickingSystem.GetTargetPosition = function(self, checkpointsTiles, cpTargetNr)
	local targetPositionX, targetPositionY
	
	for k = 1, #checkpointsTiles do
		local target = self:GetComponent(checkpointsTiles[k], "Checkpoint", 0):GetInt()
		
		if target == cpTargetNr then
			targetPositionX, targetPositionY = self:GetComponent(checkpointsTiles[k], "MapPosition", 0):GetInt2()

		end
	end
	
	return targetPositionX, targetPositionY
end

AiCardPickingSystem.GetAIsCardSet = function(self, AI, Cards)

		local aisCard = {}
		local aiNr = self:GetComponent(AI, "PlayerNumber", 0):GetInt()
		for j = 1, #Cards do
			local card = self:GetComponent(Cards[j], "DealtCard", 0)
			local id = card:GetInt()
			local plyNr = self:GetComponent(id, "PlayerNumber", 0):GetInt()

			if plyNr == aiNr then
				
				aisCard[#aisCard+1] = Cards[j]
			end
		end
		
		return aisCard
end
AiCardPickingSystem.TryMove = function(self, CardSetAI, card, pos, dir)
	local cardpicked
	for j = 1, #CardSetAI do
		if CardSetAI[j] == card[1] and j <= #CardSetAI then
			pos = pos + dir
			table.remove(card, 1)
			cardpicked = CardSetAI[j]
			table.remove(CardSetAI, j)
			j = 100
		end
	end
	return cardpicked
end
AiCardPickingSystem.AIPickCards = function( self, CardSetAI, dirX, dirY, posX, posY, targetX, targetY )
	
	local pickedcards = {}
	if #CardSetAI >= 5 then
		local forwards = self:GetAllCardsOf(CardSetAI, "Forward")
		local backwards = self:GetAllCardsOf(CardSetAI, "Backward")
		local turnLefts = self:GetAllCardsOf(CardSetAI, "TurnLeft")
		local turnRights = self:GetAllCardsOf(CardSetAI, "TurnRight")
		local turnArounds = self:GetAllCardsOf(CardSetAI, "TurnAround")

		for i = 1, 5 do
			
			if posY < targetY and dirY == 1 and #forwards > 0 then

				pickedcards[#pickedcards+1] = self:TryMove(CardSetAI, forwards, posY, dirY)

			elseif posY < targetY and dirY == -1 and #backwards > 0 then

				pickedcards[#pickedcards+1] = self:TryMove(CardSetAI, backwards, posY, dirY)

			elseif posY > targetY and dirY == -1 and #forwards > 0 then

				pickedcards[#pickedcards+1] = self:TryMove(CardSetAI, forwards, posY, dirY)

			elseif posY > targetY and dirY == 1 and #backwards > 0 then

				pickedcards[#pickedcards+1] = self:TryMove(CardSetAI, backwards, posY, dirY)

			elseif posX < targetX and dirX == 1 and #forwards > 0 then

				pickedcards[#pickedcards+1] = self:TryMove(CardSetAI, forwards, posX, dirX)

			elseif posX < targetX and dirX == -1 and #backwards > 0 then
				
				pickedcards[#pickedcards+1] = self:TryMove(CardSetAI, backwards, posX, dirX)

			elseif posX > targetX and dirX == -1 and #forwards > 0 then

				pickedcards[#pickedcards+1] = self:TryMove(CardSetAI, forwards, posX, dirX)

			elseif posX > targetX and dirX == 1 and #backwards > 0 then
				
				pickedcards[#pickedcards+1] = self:TryMove(CardSetAI, backwards, posX, dirX)

			elseif posX > targetX and dirX == 1 and #turnArounds > 0 then

				pickedcards[#pickedcards+1] = self:TryMove(CardSetAI, turnArounds, 0, 0)
				dirX = - dirX

			elseif posX < targetX and dirX == -1 and #turnArounds > 0 then

				pickedcards[#pickedcards+1] = self:TryMove(CardSetAI, turnArounds, 0, 0)
				dirX = - dirX

			elseif posY < targetY and dirY == -1 and #turnArounds > 0 then

				pickedcards[#pickedcards+1] = self:TryMove(CardSetAI, turnArounds, 0, 0)
				dirY = - dirY

			elseif posY > targetY and dirY == 1 and #turnArounds > 0 then

				pickedcards[#pickedcards+1] = self:TryMove(CardSetAI, turnArounds, 0, 0)
				dirY = - dirY

			else
				local cardNr = math.random(1, #CardSetAI)
			
				local pickedcard = CardSetAI[cardNr]
				
				local Cardname = self:GetComponent(CardSetAI[cardNr], "CardAction", 0):GetString()

				if	Cardname ==	"Forward" or Cardname == "Backward"	then
					posX = posX + dirX
					posY = posY + dirY
				elseif	Cardname ==	"TurnLeft"		then
					dirY = dirX
					dirX = -dirY
				elseif	Cardname ==	"TurnRight"		then
					dirY = -dirX
					dirX = dirY
				elseif	Cardname ==	"TurnAround"	then
					dirY = -dirX
					dirX = -dirY
				end

			
				pickedcards[#pickedcards + 1] = pickedcard

				table.remove(CardSetAI, cardNr)
			end

			
			
		end
	end
	return pickedcards
end

AiCardPickingSystem.GetAllCardsOf = function( self, CardSetAI, cardName )
	
	local cards = {}
	for i = 1, #CardSetAI do

		local nameCard = self:GetComponent(CardSetAI[i], "CardAction", 0):GetString()

		if cardName == nameCard then
			cards[#cards + 1] = CardSetAI[i]
		end
	end
	return cards
end

AiCardPickingSystem.SendCards = function(self, pickedcards, player)
	
	print("DONE")
	local unit = world:GetComponent(player, "UnitEntityId", "Id"):GetInt()
	
	world:CreateComponentAndAddTo("HasSelectedCards", player)
	world:CreateComponentAndAddTo("UnitSelectedCards", unit)

	for i = 1, self.NumberOfCardsToPick do
		local action = world:GetComponent(pickedcards[i], "CardAction", 0):GetString()
		local prio = world:GetComponent(pickedcards[i], "CardPrio", 0):GetInt()
		--print("AI Action: " .. action .. " - Prio: " .. prio)
	
		world:RemoveComponentFrom("DealtCard", pickedcards[i])
		world:CreateComponentAndAddTo("CardStep", pickedcards[i])
		world:SetComponent(pickedcards[i], "CardStep", "Step", i)
		world:SetComponent(pickedcards[i], "CardStep", "UnitEntityId", unit)
		
	end
end

AiCardPickingSystem.OnEntityAdded = function(self, entity)

	if world:EntityHasComponent(entity, "DealtCard") then
		local playerid = self:GetComponent(entity, "DealtCard", 0)
		local id = playerid:GetInt()
		local plynum = self:GetComponent(id, "PlayerNumber", 0):GetInt()
		local card = self:GetComponent(entity, "CardAction", 0):GetString()
		--print ( plynum .. " gets a " .. card .. " Card" )
	elseif world:EntityHasComponent(entity, "TileComp") then
	
	end
end