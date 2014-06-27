require("luaxmt")

XMT = {}
XMT.NOTEOFF = 108
XMT.NO_LOOP = 0
XMT.FORWARD_LOOP = 1
XMT.PING_PONG = 2
XMT.BIT_16 = 4
function makenote(nn, ins, vol, fx, param)
    return {nn = nn or -1, ins = ins or -1, vol = vol or -1, 
    fx = fx or -1, param = param or -1}
end

XMT.PARAMS = {
    nchan = xm_set_nchan
}

function XMT:create(args)
    args = args or nil
    o = args.o or {}

    setmetatable(o, self)
    self.__index = self
    p = xm_init_xm_params()
  
    for param, val in pairs(args) do
        XMT.PARAMS[param](p, val)
    end

    o.xm = xm_new(p)

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

function XMT:addbuffer(ins, buf)
    xm_addbuffer(self.xm, ins, #buf, buf)
end

function XMT:transpose(ins, samp, nn, fine)
    fine = fine or 0
    xm_transpose(self.xm, ins, samp, nn, fine)
end

function XMT:write(filename)
    xm_write(self.xm, filename)
end

function XMT:update_ptable(pos, pnum)
    xm_update_ptable(self.xm, pos, pnum)
end

function XMT:set_loop_mode(ins, samp, mode)
    xm_set_loop_mode(self.xm, ins, samp, mode)
end

