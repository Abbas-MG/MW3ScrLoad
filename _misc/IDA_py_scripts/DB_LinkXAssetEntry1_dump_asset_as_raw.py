import idaapi


# bp at 0x05857C3
# ESI has the "XAsset"
esi = idc.get_reg_value("esi")

# get RawFile* from XAsset
RawFile_ptr = idc.get_wide_dword(esi + 4)

# get fileName from RawFile
fileName_ptr    = idc.get_wide_dword(RawFile_ptr + 0)
fileName_bytes  = ida_bytes.get_bytes(fileName_ptr, 200)
# decode() is not zero terminated
fileName_string = fileName_bytes.split(b'\x00')[0].decode('ascii', errors='ignore')

# skip problematic file
if fileName_string == 'codescripts/$default':
    fileName_string = 'codescripts_default'


print(fileName_string)


RF_len = Appcall.DB_GetRawFileLen(RawFile_ptr)
if RF_len < 2:
    print("fileName too short")
    sys.exit(1)


#print(f"RawFileLen +1 = {hex(RF_len)}")

# allocate mem and use it to decompress rawFile buffer
mem = Appcall.Hunk_AllocateTempMemoryHigh(RF_len)
Appcall.DB_GetRawBuffer(RawFile_ptr, mem, RF_len)

# convert mem's data to string
raw_bytes = ida_bytes.get_bytes(mem, RF_len)
content_str = raw_bytes.split(b'\x00')[0].decode('ascii', errors='ignore')

# free the allocated mem
Appcall.Hunk_ClearTempMemoryHigh()


cwd = os.getcwd()
dumpDir = "RawFileDump_2"
os.makedirs( dumpDir, exist_ok=True)
file_path = os.path.join(cwd, dumpDir, fileName_string)

try:
    log_file = open(file_path, "w+") # overwrite
    log_file.write(content_str)
    log_file.close()
except Exception as e:
    #print(f"Appcall failed: {str(e)}")
    print(f"fail on File: {fileName_string}")


# Optionally, continue execution
