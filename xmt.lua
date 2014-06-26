require("luaxmt")

XMT = {}

function makenote(nn, ins, vol, fx, param)
    return {nn = nn or -1, ins = ins or -1, vol = vol or -1, 
    fx = fx or -1, param = param or -1}
end

function XMT:create(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    o.xm = xm_new()
    return o
end

function XMT:addins()
    return xm_addinstrument(self.xm)
end

function XMT:addnote(patnum, chan, row, note)
    xm_addnote(self.xm, patnum, chan, row, 
    xm_makenote(note.nn, note.ins, note.vol, note.fx, note.param))
end

function XMT:addsample(ins, file)
    xm_addsample(self.xm, ins, file)
end

function XMT:write(filename)
    xm_write(self.xm, filename)
end
--xm = xm_new()
--instr = xm_addinstrument(xm)
--xm_addsample(xm, inst, "samples/pad8bit8khz.wav")
--xm_addnote(xm, 0, 0, 0, xm_makenote(60, instr + 1, 0, 0, 0))
--xm_write(xm, "out.xm")
--

x = XMT:create()
ins1 = x:addins()
print(ins1)
x:addsample(ins1, "samples/pad8bit8khz.wav")
x:addnote(0, 0, 0, makenote(60, 1))
x:write("out.xm")
