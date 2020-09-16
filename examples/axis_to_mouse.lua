Mouse = {}
Mouse.__index = Mouse

function Mouse:new( speed )
    local m = {}
    setmetatable( m, Mouse )
    m.speed = speed
    m.x = 0
    m.y = 0
    m.velocityX = 0
    m.velocityY = 0
    return m
end

function Mouse:update( deltaTime )
    self.x = self.x + self.velocityX * deltaTime
    local x = math.floor( self.x )
    self.x = self.x - x

    self.y = self.y + self.velocityY * deltaTime
    local y = math.floor( self.y )
    self.y = self.y - y

    GGPAD_mouseMove( x, y )
end

function Mouse:setX( value )
    self.velocityX = self.speed * value / 0x7fff
end

function Mouse:setY( value )
    self.velocityY = self.speed * value / 0x7fff
end

-- moves mouse by 400px per second at max stick tilt
local mouse = Mouse:new( 400 )

GGPAD_event = function( btn, value )
    if ( btn == Gamepad.RX ) then
        mouse:setX( value )
    elseif ( btn == Gamepad.RY ) then
        mouse:setY( value )
    end
end

GGPAD_update = function( deltaTime )
    mouse:update( deltaTime )
end
