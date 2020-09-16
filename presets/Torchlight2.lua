-- for accurately positioning mouse at screen center
local screenSize = {}
screenSize.w = 1920
screenSize.h = 1080

local mapping = {}

-- action bar skills
mapping[ Gamepad.X ] = Keyboard._1
mapping[ Gamepad.Y ] = Keyboard._2
mapping[ Gamepad.A ] = Keyboard._3
mapping[ Gamepad.B ] = Keyboard._4

-- toggle between map modes
mapping[ Gamepad.SELECT ] = Keyboard.M

-- extra mapping
mapping[ Gamepad.R1 ] = Keyboard.LSHIFT
mapping[ Gamepad.R3 ] = Mouse.RIGHT
mapping[ Gamepad.L3 ] = Mouse.LEFT


local mouse = {}
mouse.speed = 180
mouse.x = 0
mouse.y = 0
mouse.l = {}
mouse.l.x = 0
mouse.l.y = 0
mouse.l.nx = 0
mouse.l.ny = 0
mouse.r = {}
mouse.r.x = 0
mouse.r.y = 0
mouse.r.nx = 0
mouse.r.ny = 0

-- cycling between character menu pages when pressing Gamepad.START
local leftPanel = {}
leftPanel[ 0 ] = Keyboard.P
leftPanel[ 1 ] = Keyboard.C
leftPanel[ 2 ] = Keyboard.J
local rightPanel = {}
rightPanel[ 0 ] = Keyboard.I
rightPanel[ 1 ] = Keyboard.S
rightPanel[ 2 ] = Keyboard.Q
local panelIndex = 0
local showPanels = function( arg )
    -- don't do anything when releasing gamepad button
    if ( arg == 0 ) then
        return
    end
    -- when cycle count equals 3, hide the panels and reset counting
    if ( panelIndex == 3 ) then
        GGPAD_keyboardSet( Keyboard.ESC, 1 )
        GGPAD_keyboardSet( Keyboard.ESC, 0 )
        panelIndex = 0
        return
    end

    GGPAD_keyboardSet( leftPanel[ panelIndex ], 1 )
    GGPAD_keyboardSet( rightPanel[ panelIndex ], 1 )
    GGPAD_keyboardSet( leftPanel[ panelIndex ], 0 )
    GGPAD_keyboardSet( rightPanel[ panelIndex ], 0 )
    panelIndex = panelIndex + 1
end

-- move character in specific direction when tilting left stick,
-- by placing mouse cursor on a circle around character and hitting mouse button.
-- this will occur only when stick tilt is at least 80%
local circle = {}
circle.radiusNormal = 70
circle.radiusExtraRange = 300
circle.radius = circle.radiusNormal

local moveCharacter = function( dt )
    if ( math.sqrt( mouse.l.nx * mouse.l.nx + mouse.l.ny * mouse.l.ny ) < 0.8 ) then
        return
    end
    local angle = math.atan2( mouse.l.nx, mouse.l.ny )
    local px = math.sin( angle ) * circle.radius + screenSize.w / 2
    local py = math.cos( angle ) * circle.radius + screenSize.h / 2
    GGPAD_mouseSet( px, py )
    GGPAD_keyboardSet( Mouse.LEFT, 1 )
    GGPAD_keyboardSet( Mouse.LEFT, 0 )
end


-- deadzonify analog stick
local applyDeadzone = function( axis, range )
    local length = math.sqrt( axis.x * axis.x + axis.y * axis.y )
    if ( length < range ) then
        axis.nx = 0
        axis.ny = 0
        return
    end
    local angle = math.atan2( axis.x, axis.y )
    local newLenght = ( length - range ) / ( 1.0 - range )
    axis.nx = math.sin( angle ) * newLenght
    axis.ny = math.cos( angle ) * newLenght
end

-- mapping of functions to do stuff, instead of keyboard keys
local mapping2 = {}
mapping2[ Gamepad.LX ] = function( a ) mouse.l.x = a / 0x7fff applyDeadzone( mouse.l, 0.1618 ) end
mapping2[ Gamepad.LY ] = function( a ) mouse.l.y = a / 0x7fff applyDeadzone( mouse.l, 0.1618 ) end
mapping2[ Gamepad.RX ] = function( a ) mouse.r.x = a / 0x7fff applyDeadzone( mouse.r, 0.1618 ) end
mapping2[ Gamepad.RY ] = function( a ) mouse.r.y = a / 0x7fff applyDeadzone( mouse.r, 0.1618 ) end
mapping2[ Gamepad.R2 ] = function( a ) circle.radius = circle.radiusNormal + circle.radiusExtraRange * ( a / 0x7fff ) end
mapping2[ Gamepad.START ] = showPanels


GGPAD_event = function( a_btn, value )
    local btn = mapping[ a_btn ]
    if ( btn ) then
        GGPAD_keyboardSet( btn, value )
        return
    end

    local func = mapping2[ a_btn ]
    if ( func ) then
        func( value )
        return
    end
end

GGPAD_update = function( a_deltaTime )
    mouse.x = mouse.x + mouse.speed * mouse.r.nx * a_deltaTime;
    local x = math.floor( mouse.x )
    mouse.x = mouse.x - x

    mouse.y = mouse.y + mouse.speed * mouse.r.ny * a_deltaTime;
    local y = math.floor( mouse.y )
    mouse.y = mouse.y - y
    if ( mouse.x or mouse.y ) then
        GGPAD_mouseMove( x, y )
    end
    moveCharacter( a_deltaTime )
end
