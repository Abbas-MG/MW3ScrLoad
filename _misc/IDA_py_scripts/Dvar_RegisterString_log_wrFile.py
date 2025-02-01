import idaapi
import idc
import idautils

# bp at 0x0519822
# Get the value at EAX (relative to the current ESP)
eax = idc.get_reg_value("eax")

# Dereference the pointer to get the C-string of Dvar
c_string = idc.get_strlit_contents(eax, -1, idc.STRTYPE_C)


# get Value being passed
esp = idc.get_reg_value("esp")
esp_ptr = idc.get_wide_dword(esp + 4) # compensate for "push 7"
value_c_string = idc.get_strlit_contents(esp_ptr, -1, idc.STRTYPE_C)

logStr = ""

if c_string:
    logStr = "SetString: " + c_string.decode()

if value_c_string:
    logStr += "  -->>  " + value_c_string.decode()

if not c_string:
    log_file.write("Failed to dereference pointer.\n")
    log_file.close()
    

log_file = open("Log_Dvar_SetStringByName.txt", "a")
log_file.write(logStr + "\n")   
log_file.close()


# Optionally, continue execution
idaapi.continue_process()
