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

function moveMouseX( a_value )
    GGPAD_mouseMove( Mouse.X, math.floor( a_value * 0.1 ) )
end
function moveMouseY( a_value )
    GGPAD_mouseMove( Mouse.Y, math.floor( a_value * -0.1 ) )
end

mapping2 = {}
mapping2[ Gamepad.LX ] = moveMouseX
mapping2[ Gamepad.LY ] = moveMouseY

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
