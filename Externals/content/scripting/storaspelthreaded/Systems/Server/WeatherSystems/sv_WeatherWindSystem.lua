WeatherWindSystem	=	System()
WeatherWindSystem.CurrentStep	=	0

WeatherWindSystem.Initialize = function(self)
	--	Set Name
	self:SetName("WeatherWindSystem")
	
	--	Toggle EntitiesAdded
	self:UsingEntitiesAdded()

	--	Set Filter
	self:AddComponentTypeToFilter("NewStep", FilterType.RequiresOneOf)
	self:AddComponentTypeToFilter("WeatherWind", FilterType.RequiresOneOf)
	self:AddComponentTypeToFilter("Unit", FilterType.RequiresOneOf)
end

WeatherWindSystem.EntitiesAdded = function(self, dt, taskIndex, taskCount, newEntities)



	for n = 1, #newEntities do
		local	tEntity	=	newEntities[n]
		
		if world:EntityHasComponent(tEntity, "WeatherWind") then
		
			--	Randomize which round to play it on
			local	numberOfSteps	=	5+1 - self.CurrentStep%5
			local	stepToTrigger	=	math.random(1, 5)
			
			world:GetComponent(tEntity, "Weather", "StepToHappen"):SetInt(1)
			world:GetComponent(tEntity, "Weather", "StageToHappen"):SetInt(0)
			world:GetComponent(tEntity, "WeatherWind", "Force"):SetInt(1)
			
			local	dirX, dirZ	=	0,0
			if math.random(1, 100) <= 50 then
				dirX	=	(-1)^math.random(1, 10)
			else
				dirZ	=	(-1)^math.random(1, 10)
			end
			
			world:CreateComponentAndAddTo("Direction", tEntity)
			world:GetComponent(tEntity, "Direction", "X"):SetInt(1)
			world:GetComponent(tEntity, "Direction", "Z"):SetInt(0)
			
		elseif world:EntityHasComponent(tEntity, "NewStep") then
			
			self.CurrentStep	=	self.CurrentStep+1
		end
		
	end
	
	--	Get current weather
	local	currentWind	=	self:GetEntities("WeatherWind")
	if #currentWind ~= 0 then
		print("NUMBER OF WEATHERS: " .. #currentWind)
		self:TickWeather(currentWind[1])
	end
	
end

WeatherWindSystem.TickWeather = function(self, weatherEntity)
	
	--	Reduce steps left
	local	stepsLeft	=	world:GetComponent(weatherEntity, "Weather", "StepToHappen"):GetInt()
	world:GetComponent(weatherEntity, "Weather", "StepToHappen"):SetInt(stepsLeft-1)
	
	if stepsLeft-1 <= 0 then
	
		local	allUnits	=	self:GetEntities("Unit")
		local	dirX, dirZ	=	world:GetComponent(weatherEntity, "Direction", "X"):GetInt2()
		local	windForce	=	world:GetComponent(weatherEntity, "WeatherWind", "Force"):GetInt()
		
		print("Wind direction " .. dirX .. ", " .. dirZ)
		
		for i = 1, #allUnits do
		
			
			local	tUnit	=	allUnits[i]
			
			if not world:EntityHasComponent(tUnit, "UnitDead") then
			
				local	posX, posZ	=	world:GetComponent(tUnit, "MapPosition", "X"):GetInt2()
				
				local id = world:CreateNewEntity()
				world:CreateComponentAndAddTo("SimultaneousMove", id)
				world:SetComponent(id, "SimultaneousMove", "Unit", tUnit)
				world:SetComponent(id, "SimultaneousMove", "PosX", posX + dirX)
				world:SetComponent(id, "SimultaneousMove", "PosZ", posZ + dirZ)
				world:SetComponent(id, "SimultaneousMove", "DirX", dirX)
				world:SetComponent(id, "SimultaneousMove", "DirZ", dirZ)
				world:SetComponent(id, "SimultaneousMove", "Steps", windForce)
			end
		end
		
		
		print("WIND!!!")
	
		world:KillEntity(weatherEntity)
	end
	
	
	
	
end









