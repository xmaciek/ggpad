local mapping = {}
mapping[ Gamepad.UP ] = Keyboard.W
mapping[ Gamepad.DOWN ] = Keyboard.S
mapping[ Gamepad.LEFT ] = Keyboard.A
mapping[ Gamepad.RIGHT ] = Keyboard.D

mapping[ Gamepad.X ] = Keyboard.J
mapping[ Gamepad.Y ] = Keyboard.I
mapping[ Gamepad.A ] = Keyboard.K
mapping[ Gamepad.B ] = Keyboard.L

function GGPAD_buttonChanged( a_btn, a_value )
    local btn = mapping[ a_btn ]
    if ( btn ) then
        GGPAD_setKeyboard( btn, a_value )
    end
end
