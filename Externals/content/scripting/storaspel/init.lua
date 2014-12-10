-- Components
package.path = package.path .. ";../../../Externals/content/scripting/storaspel/components/?.lua"
require "components"

-- Systems
package.path = package.path .. ";../../../Externals/content/scripting/storaspel/systems/?.lua"
require "movementsystem"
require "networkmessagessystem"

worldCreator:AddSystemGroup()
worldCreator:AddSystemToCurrentGroup(MovementSystem)
worldCreator:AddSystemToCurrentGroup(networkMessagesSystem)