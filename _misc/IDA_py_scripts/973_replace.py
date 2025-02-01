import idaapi

# bp at 0x0446A96 (preParse Func)

#curr_addr = get_screen_ea()
eax = idc.get_reg_value("eax")

# IS IT THE EMPTY script? "973" is an empty sub-script of paris map; perfect for quick tests
if (Appcall.SL_ConvertToString(eax).decode("utf-8") == "973"):
	newCanonName = Appcall.Scr_CreateCanonicalFilename("sample")
	idc.set_reg_value(newCanonName, "eax")
	print("script replaced.")
	ida_dbg.disable_bpt(0x446a96)