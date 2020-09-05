local mapping = {}
mapping[ Gamepad.UP ] = Keyboard.W
mapping[ Gamepad.DOWN ] = Keyboard.S
mapping[ Gamepad.LEFT ] = Keyboard.A
mapping[ Gamepad.RIGHT ] = Keyboard.D

mapping[ Gamepad.X ] = Keyboard.J
mapping[ Gamepad.Y ] = Keyboard.I
mapping[ Gamepad.A ] = Keyboard.K
mapping[ Gamepad.B ] = Keyboard.L
mapping[ Gamepad.START ] = Mouse.LEFT

local mouse = {}
mouse.x = 0
mouse.y = 0
mouse.dx = 0
mouse.dy = 0

local mapping2 = {}
mapping2[ Gamepad.LX ] = function( a_value ) mouse.dx = 400 * a_value / 0x7fff end
mapping2[ Gamepad.LY ] = function( a_value ) mouse.dy = 400 * a_value / 0x7fff end


function GGPAD_event( a_btn, a_value )
    local btn = mapping[ a_btn ]
    if ( btn ) then
        GGPAD_keyboardSet( btn, a_value )
        return
    end

    local axis = mapping2[ a_btn ]
    if ( axis ) then
        axis( a_value )
        return
    end
end

function GGPAD_update( a_deltaTime )
    mouse.x = mouse.x + mouse.dx * a_deltaTime;
    local x = math.floor( mouse.x )
    mouse.x = mouse.x - x

    mouse.y = mouse.y + mouse.dy * a_deltaTime;
    local y = math.floor( mouse.y )
    mouse.y = mouse.y - y
    GGPAD_mouseMove( x, y )
end

GGPAD_nativeEvent_ = function( atype, code, value )
    local map = {}
    if ( not map[ code ] ) then print( atype .. " " .. code .. " " .. value ) end
end
