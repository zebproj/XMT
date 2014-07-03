--[[
"Random Blips"
Paul Batchelor
July 2014
--]]
require("xmt")

math.randomseed(os.time())

x = XMT:create({nchan = 2, speed = 1})

function create_square(size, pw)
    local tbl = {}
    for i = 1, size, 1 do
        if(i < size * pw) then
            table.insert(tbl, 1)
        else
            table.insert(tbl, -1)
        end
    end
    return tbl
end

numpats = 4
patlen = 0x80

--Nested XMT functions which create a new square wave instrument
square = x:set_loop_mode(x:addbuf(x:addins(), create_square(64, 0.5)), XMT.FORWARD)

for i = 1, numpats, 1 do
--Create a new pattern
    pat = x:addpat(patlen)
--Add new pattern to the end of the song
    x:update_ptable(i - 1, pat)
--Add random notes between 60 (C-4) and 84 (C-6) on new pattern
    for r = 1, patlen, 1 do
        x:addnote(pat, 0, r - 1, 
        makenote(60 + math.random(24), square.ins, 0x10 + math.random(0x40)))
    end
end

x:write("ex3.xm")
