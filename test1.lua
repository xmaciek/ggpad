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
mapping2[ Gamepad.LX ] = function( a_value ) mouse.dx = 400 * a_value / 127 end
mapping2[ Gamepad.LY ] = function( a_value ) mouse.dy = 400 * -a_value / 127 end


function GGPAD_buttonChanged( a_btn, a_value )
    local btn = mapping[ a_btn ]
    if ( btn ) then
        GGPAD_setKeyboard( btn, a_value )
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
    if ( x ~= 0 ) then
        mouse.x = mouse.x - x
        GGPAD_mouseMove( Mouse.X, x )
    end

    mouse.y = mouse.y + mouse.dy * a_deltaTime;
    local y = math.floor( mouse.y )
    if ( y ~= 0 ) then
        mouse.y = mouse.y - y
        GGPAD_mouseMove( Mouse.Y, y )
    end
end
