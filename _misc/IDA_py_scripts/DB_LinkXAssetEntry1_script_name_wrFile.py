import idaapi

# bp at 0x05855DC
eax = idc.get_reg_value("eax")

ScriptFile_name_bytes  = ida_bytes.get_bytes(eax, 60)
#ScriptFile_name_string = ScriptFile_name_bytes.split(b'\x00')[0].decode('ascii', errors='ignore')
ScriptFile_name_string = ScriptFile_name_bytes.decode('ascii', errors='ignore')


log_file = open("Log_LinkAsset_0.txt", "a")
log_file.write(ScriptFile_name_string + "\n")
log_file.close()


return False