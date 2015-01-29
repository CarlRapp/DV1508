CardPickedPositionSystem = System()
CardPickedPositionSystem.UpOffset = -1

CardPickedPositionSystem.Initialize = function(self)
	--	Set Name
	self:SetName("CardPickedPositionSystem")
	
	--	Toggle EntitiesAdded
	self:UsingEntitiesAdded()
	
	--	Set Filter
	self:AddComponentTypeToFilter("Position", FilterType.Mandatory)
	self:AddComponentTypeToFilter("Rotation", FilterType.Mandatory)
	self:AddComponentTypeToFilter("CardIndex", FilterType.Mandatory)
	self:AddComponentTypeToFilter("CardSelected", FilterType.Mandatory)
end

CardPickedPositionSystem.EntitiesAdded = function(self, dt, taskIndex, taskCount, newEntities)

	local entities = self:GetEntities()
	for i = 1, #entities do
		local card = entities[i]
	
		local data = self:GetComponent(card, "SelectCard", "Index"):GetInt()
		
		local halfentities = #entities/2
		local px = (-halfentities + data - 0.5) * 0.7
		local py = self.UpOffset
		local pz = -6
		
		if not world:EntityHasComponent(card, "LerpTargetPosition") then
			world:CreateComponentAndAddTo("LerpTargetPosition", card)
		end
		
		local position = self:GetComponent(card, "LerpTargetPosition", 0)
		position:SetFloat3(px, py, pz)
		
		if not world:EntityHasComponent(card, "LerpTime") then
			world:CreateComponentAndAddTo("LerpTime", card)
		end
		
		local timer = self:GetComponent(card, "LerpTime", 0)
		timer:SetFloat(0.5)
	
	end
	
end

