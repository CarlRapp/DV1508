GameRunning = false;


-- Components
package.path = package.path .. ";../../../Externals/content/scripting/lobby/components/?.lua"
require "components"
require "interfacecomponents"
require "buttoncomponents"

-- Systems
package.path = package.path .. ";../../../Externals/content/scripting/lobby/systems/?.lua"
require "systems"

require "sh_pickboxsystem"
require "sh_buttonhoversystem"
require "sh_buttonpressedsystem"

--if Server then
	require "serverlobbysystem"
	require "serverconnectsystem"
--end

--if Client then
	require "clientlobbysystem"
	require "clientconnectsystem"
--end

-- Templates
package.path = package.path .. ";../../../Externals/content/scripting/lobby/templates/?.lua"
require "templates"

worldCreator:AddSystemGroup()
worldCreator:AddSystemToCurrentGroup(StartUpSystem)

worldCreator:AddSystemToCurrentGroup(PickBoxSystem)
worldCreator:AddSystemToCurrentGroup(ButtonPressedSystem)
worldCreator:AddSystemToCurrentGroup(ButtonHoverSystem)

--if Server then
	worldCreator:AddSystemToCurrentGroup(ServerLobbySystem)
	worldCreator:AddSystemToCurrentGroup(ServerConnectSystem)
--end

--if Client then
	worldCreator:AddSystemToCurrentGroup(ClientLobbySystem)
	worldCreator:AddSystemToCurrentGroup(ClientConnectSystem)
--end