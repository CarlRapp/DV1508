CardHoverSystem = System()
CardHoverSystem.Scale = 1.1
CardHoverSystem.UpOffset = -0.2

CardHoverSystem.Update = function(self, dt)

	local entities = self:GetEntities()
	for i = 1, #entities do
		local entity = entities[i]
    
		local action = self:GetComponent(entity, "CardAction", 0):GetString()
		local scale = self:GetComponent(entity, "Scale", 0)
		local prio = self:GetComponent(entity, "CardPrio", 0):GetInt()
		
		graphics:RenderSimpleText(action, 0,44)
		graphics:RenderSimpleText(prio, 15,44)
		
		scale:SetFloat3(self.Scale, self.Scale, self.Scale)
		
	end
		
end

CardHoverSystem.Initialize = function(self)
	self:SetName("Card hover System")
	self:AddComponentTypeToFilter("CardPrio", FilterType.Mandatory)
	self:AddComponentTypeToFilter("CardAction", FilterType.Mandatory)
	self:AddComponentTypeToFilter("OnPickBoxHit", FilterType.Mandatory)
	
	print("Card hover System initialized!")
end