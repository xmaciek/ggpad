AxisToKeyboard = {}
AxisToKeyboard.__index = AxisToKeyboard

function AxisToKeyboard:new( keyA, keyB )
    local ret = {}
    setmetatable( ret, AxisToKeyboard )
    ret.axis = 0.0
    ret.keyA = keyA
    ret.keyB = keyB
    ret.timePassed = 0.0
    ret.freqTime = 1.0 / 200.0
    ret.isPressed = false
    return ret
end

function AxisToKeyboard:getKey( axis )
    if ( self.axis < 0.0 ) then
        return self.keyA
    else
        return self.keyB
    end
end

function AxisToKeyboard:setAxis( value )
    self.axis = value
end

function AxisToKeyboard:update( deltaTime )
    self.timePassed = math.fmod( self.timePassed + deltaTime, self.freqTime * 10 )
    local releaseTime = math.abs( self.axis * self.freqTime * 10 )

    if ( releaseTime <= self.freqTime ) then
        if ( self.isPressed ) then
            self.isPressed = false
            GGPAD_keyboardSet( self.keyA, 0 )
            GGPAD_keyboardSet( self.keyB, 0 )
        end
        return
    end

    if ( self.timePassed < self.freqTime ) then
        self.isPressed = true
        GGPAD_keyboardSet( self:getKey(), 1 )
    end

    if ( self.timePassed >= releaseTime ) then
        GGPAD_keyboardSet( self:getKey(), 0 )
    end
end

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

local dzl = Deadzone:new( 0.1618 )
local dzr = Deadzone:new( 0.1618 )
local lx = AxisToKeyboard:new( Keyboard.A, Keyboard.D )
local ly = AxisToKeyboard:new( Keyboard.W, Keyboard.S )
local rx = AxisToKeyboard:new( Keyboard.LEFT, Keyboard.RIGHT )
local ry = AxisToKeyboard:new( Keyboard.UP, Keyboard.DOWN )

GGPAD_event = function( button, value )
    if ( button == Gamepad.LX ) then
        dzl:setX( value )
        lx:setAxis( dzl:x() )
    elseif ( button == Gamepad.LY ) then
        dzl:setY( value )
        ly:setAxis( dzl:y() )
    elseif ( button == Gamepad.RX ) then
        dzr:setX( value )
        rx:setAxis( dzr:x() )
    elseif ( button == Gamepad.RY ) then
        dzr:setY( value )
        ry:setAxis( dzr:y() )
    end
end

GGPAD_update = function( deltaTime )
    lx:update( deltaTime )
    ly:update( deltaTime )
    rx:update( deltaTime )
    ry:update( deltaTime )
end
