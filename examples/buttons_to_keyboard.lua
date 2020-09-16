local keyboard = {}

keyboard[ Gamepad.A ] = Keyboard.K
keyboard[ Gamepad.B ] = Keyboard.L
keyboard[ Gamepad.X ] = Keyboard.J
keyboard[ Gamepad.Y ] = Keyboard.I
keyboard[ Gamepad.L1 ] = Mouse.LEFT
keyboard[ Gamepad.R1 ] = Mouse.RIGHT


GGPAD_event = function( button, value )
    local btn = keyboard[ button ]
    if ( btn ) then
        GGPAD_keyboardSet( btn, value )
    end
end
