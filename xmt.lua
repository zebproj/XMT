require("luaxmt")

xm = xm_new()
instr = xm_addinstrument(xm)
xm_addsample(xm, inst, "samples/pad8bit.wav")
xm_addnote(xm, 0, 0, 0, xm_makenote(60, instr + 1, 0, 0, 0))
xm_write(xm, "out.xm")
