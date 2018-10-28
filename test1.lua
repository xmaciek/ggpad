mapping = {}
mapping[ Gamepad.X ] = 1234


function GGPAD_buttonChanged( a_btn, a_value )
    if ( mapping[ a_btn ] ) then
        print( "got " .. a_btn .. " mapped to value " .. mapping[ a_btn ] .. " and has new state " .. a_value )
    else
        print( "unknown button " .. a_btn )
    end
end
