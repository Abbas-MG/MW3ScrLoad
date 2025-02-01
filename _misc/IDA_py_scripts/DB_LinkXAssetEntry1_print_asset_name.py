import idaapi
import idc

# bp at 0x05857C0
# Get the value of esp
esp_value = idc.get_reg_value("esp")


value_at_esp_plus_8 = idc.get_wide_dword(esp_value + 8)
str_ptr = idc.get_wide_dword(value_at_esp_plus_8 + 0)
str = idc.get_wide_dword(str_ptr + 0)


asset_type = idc.get_wide_dword(esp_value + 4)

# skip if not script nor raw
if asset_type != 0x26 and asset_type != 0x27:
    return False


fileName_bytes  = ida_bytes.get_bytes(str, 200)

if fileName_bytes is not None:
    fileName_string = fileName_bytes.decode('ascii', errors='ignore')
    print(fileName_string)
    return False
else:
    return False
    




