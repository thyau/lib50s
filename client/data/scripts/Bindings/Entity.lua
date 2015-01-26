print("Entity Loaded")
Entity = {}
Entity.__index = Entity

function Entity.Entity()
    local Ent = {}
    setmetatable(Ent,Entity)
    Ent._ptr = _C_Entity_Create()
    return Ent
end

function Entity.FromPtr(ptr)
    local Ent = {}
    setmetatable(Ent,Entity)
    Ent._ptr = ptr
    return Ent
end

function Entity:GetPosition()
    return _C_Entity_GetPosition(self._ptr)
end

function Entity:GetRotation()
    return _C_Entity_GetRotation(self._ptr)
end

function Entity:SetPosition(x,y,z)
    _C_Entity_SetPosition(self._ptr,x,y,z)
end

function Entity:SetRotation(x,y,z)
    _C_Entity_Rotation(self._ptr,x,y,z)
end