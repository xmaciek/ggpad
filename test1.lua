local mappingDigital = {}
mappingDigital[ Gamepad.X ] = "X"
mappingDigital[ Gamepad.Y ] = "Y"
mappingDigital[ Gamepad.A ] = "A"
mappingDigital[ Gamepad.B ] = "B"
mappingDigital[ Gamepad.START ] = "START"
mappingDigital[ Gamepad.SELECT ] = "SELECT"
mappingDigital[ Gamepad.UP ] = "UP"
mappingDigital[ Gamepad.DOWN ] = "DOWN"
mappingDigital[ Gamepad.LEFT ] = "LEFT"
mappingDigital[ Gamepad.RIGHT ] = "RIGHT"
mappingDigital[ Gamepad.L1 ] = "L1"
mappingDigital[ Gamepad.L3 ] = "L3"
mappingDigital[ Gamepad.R1 ] = "R1"
mappingDigital[ Gamepad.R3 ] = "R3"

local function moveMouseX( a_value )
    print( "Moving mouse X " .. a_value )
end

local function moveMouseY( a_value )
    print( "Moving mouse Y " .. a_value )
end

local mappingAnalog = {}
mappingAnalog[ Gamepad.LX ] = moveMouseX
mappingAnalog[ Gamepad.LY ] = moveMouseY
mappingAnalog[ Gamepad.L2 ] = moveMouseX
mappingAnalog[ Gamepad.R2 ] = moveMouseY

function GGPAD_buttonChanged( a_btn, a_value )
    local btn = mappingDigital[ a_btn ]
    if ( btn ) then
        print( "Got btn: " .. btn .. ",\t new value: " .. a_value )
        return
    end

    local cb = mappingAnalog[ a_btn ]
    if ( cb ) then
        cb( a_value )
        return
    end
end

local mappingNative = {}
mappingNative[ 1 ] = Gamepad.X

function GGPAD_nativeChanged( a_btn, a_value )
    local btn = mappingNative[ a_btn ]
    if ( btn ) then
        GGPAD_buttonChanged( btn, a_value )
    end
end
