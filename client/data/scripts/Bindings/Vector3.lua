print("Vector3 Loaded")

Vector3 = {}
Vector3.__index = Vector3

function Vector3:GetX()
    return _C_Vector3_GetX(self._ptr)
end

function Vector3:GetY()
    return _C_Vector3_GetY(self._ptr)
end

function Vector3:GetZ()
    return _C_Vector3_GetZ(self._ptr)
end

function Vector3:Print()
     _C_Vector3_Print(self._ptr)
end

function Vector3:Vector3(x,y,z)
    local vec = {}
    setmetatable(vec,Vector3)
    vec._ptr = _C_Vector3(x,y,z)
    return vec
end

function Vector3:FromPointer(_ptr)
    local vec = {}
    setmetatable(vec,Vector3)
    vec._ptr = _ptr
    return vec
end

function Vector3:Delete()
    _C_Vector3_Delete(self._ptr)
end

function Vector3:Set(X,Y,Z)
    _C_Vector3_Set(self._ptr,X,Y,Z)
end