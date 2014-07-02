
require("xmt")

x = XMT:create({nchan = 8})
ins1 = x:addins()
x:addsample(ins1, "samples/sine.wav")
x:addnote(0, 0, 0, makenote(72, ins1))
--x:addsample(ins1, "samples/sine.wav")
x:write("out.xm")

