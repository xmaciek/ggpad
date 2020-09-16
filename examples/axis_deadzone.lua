Deadzone = {}
Deadzone.__index = Deadzone

function Deadzone:new( value )
    local ret = {}
    setmetatable( ret, Deadzone )
    ret.value = value
    ret.rx = 0
    ret.ry = 0
    ret.nx = 0
    ret.ny = 0
    return ret
end

function Deadzone:normalize()
    local magnitude = math.sqrt( self.rx * self.rx + self.ry * self.ry ) / 0x7fff
    if ( magnitude < self.value ) then
        self.nx = 0
        self.ny = 0
        return
    end

    local angle = math.atan2( self.rx, self.ry )
    magnitude = ( magnitude - self.value ) / ( 1.0 - self.value )
    self.nx = math.sin( angle ) * magnitude
    self.ny = math.cos( angle ) * magnitude
end

function Deadzone:x()
    return self.nx
end

function Deadzone:y()
    return self.ny
end

function Deadzone:setX( value )
    self.rx = value
    self:normalize()
end

function Deadzone:setY( value )
    self.ry = value
    self:normalize()
end

-- the 0.1618 value for deadzone is arbitrary
local dzl = Deadzone:new( 0.1618 )

GGPAD_event = function( button, value )
    if ( button == Gamepad.LX ) then
        dzl:setX( value )
        -- some other function which accepts axis value
        -- foo( dzl:x() )
    end
end
