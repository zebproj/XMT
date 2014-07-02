require("xmt")
function n2m(str)
    local tbl = {}
    local note = {
        C = 0,
        D = 2,
        E = 4,
        F = 5,
        G = 7,
        A = 9,
        B = 11
    }
    local accidental = {}
    accidental["-"] = 0
    accidental["#"] = 1
    accidental["b"] = -1
    if(str == nil) then return XMT.NOTEOFF end
    for c in str:gmatch"." do
        table.insert(tbl, c)
    end
    
    return note[tbl[1]] + accidental[tbl[2]] + 12 * (tbl[3] + 1)
end

function noteblink(args)
    local n1 = n2m(args.note1)
    local n2 = n2m(args.note2)
    local xm = args.x
    local npos = 0
    local n = -1
    for i = 1, args.steps, args.blink do
        i = i - 1 + args.offset
        if(npos == 0) then n = n1 npos = 1 else n = n2 npos = 0 end
        local pos = i % 64
        local pat = math.floor(i / 64)
        xm:addnote(pat, args.chan, pos, makenote(n, args.ins, args.vol))
    end
end

chord = {"C-4", "D-4", "G-4", "B-4"}
x = XMT:create({nchan = 4})
ins1 = x:addins()
x:addsamp(ins1, "../samples/pad3.wav")
x:set_loop_mode(ins1, 0, XMT.FORWARD_LOOP)
for i = 1, 5, 1 do 
    x:create_pattern()
    x:update_ptable(i - 1, i - 1) 
end

root = 12
vol = 0x30
args = {note1 = "C-4", note2 = "B-3", x = x, steps = 0x40 * 4, ins = ins1, blink = root * 7,
chan = 0, offset = 0, length = 8, vol = vol}
noteblink(args)
args.note1 = "D-4" args.note2 = "E-4" args.blink = root * 5 args.chan = 1
args.offset = 4
noteblink(args)
args.note1 = "G-4" args.note2 = "F#4" args.blink = root * 3 args.chan = 2
args.offset = 8
noteblink(args)
args.note1 = "B-4" args.note2 = "D-5" args.blink = root args.chan = 3
args.offset = 12
noteblink(args)

vol = vol - 0x10
volstep = vol/0x30
chan = 0
for i = 0, 0x40 - 3, 1 do
    for chan = 0, 3, 1 do
        x:addnote(4, chan, i, makenote(-1, -1, -1, 0xC, vol))
    end
    if(vol ~= 0) then vol = math.floor(vol - volstep) end
end

x:write("ex1.xm")
