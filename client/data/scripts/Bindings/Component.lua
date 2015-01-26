print("Component Loaded")
Components = {}
Component = {}
Component.__index = Component

Component.Parent = nil
Component.once = false

function Component.onUpdate(myTable,deltaTime)
    
    if(not myTable.once)then
        print(myTable)
        print(deltaTime)
        Component.once = true
    end
end

function Component.onAttach(myTable,parentPtr)
    myTable.Parent = Entity.FromPtr(parentPtr)
end

function _L_RegisterComponent(ptr,type)
    --Testing for certain types is currently unimplemented.
    Components[ptr] = {}
    setmetatable(Components[ptr],Component)
end