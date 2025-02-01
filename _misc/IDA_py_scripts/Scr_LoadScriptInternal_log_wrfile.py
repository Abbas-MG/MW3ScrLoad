import idaapi
import idc
import idautils

# bp at 0x04848A8
# Address of the instruction you're interested in
addr = here()  # You can use the current address or specify it manually

# Get the value at ESP (relative to the current ESP)
esp = idc.get_reg_value("esp")
ptr = idc.get_wide_dword(esp + 0)  # Assuming 32-bit environment

# Dereference the pointer to get the C-string
c_string = idc.get_strlit_contents(ptr, -1, idc.STRTYPE_C)

# Write the string to a log file
log_file = open("Log_LoadScriptInternal.txt", "a")
if c_string:
    log_file.write(f"Dereferenced string: {c_string.decode()}\n")
else:
    log_file.write("Failed to dereference pointer.\n")
log_file.close()

# Optionally, continue execution
idaapi.continue_process()
