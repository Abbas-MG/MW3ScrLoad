import os
import zlib

def find_ff_ff_ff_ff(data, start_index):
    """
    Search backward from the given index for the byte sequence 'FF FF FF FF'.
    """
    pattern = b'\xFF\xFF\xFF\xFF'
    # Loop backward from start_index to find the pattern
    for i in range(start_index, -1, -1):
        if data[i:i + len(pattern)] == pattern:
            return i
    return -1

def process_file(file_path):
    with open(file_path, 'rb') as file:
        data = file.read()

    # Start searching for ".gsc" strings in the file data
    search_string = b'.gsc'
    offset = 0

    while True:
        # Find the index of the ".gsc" string
        gsc_index = data.find(search_string, offset)
        if gsc_index == -1:
            break  # No more ".gsc" strings found in the file

        # Find the index of the 'FF FF FF FF' pattern going backwards from the .gsc index
        start_index = find_ff_ff_ff_ff(data, gsc_index)
        if start_index == -1:
            print(f"Could not find 'FF FF FF FF' before .gsc at index {gsc_index} in {file_path}")
            offset = gsc_index + len(search_string)
            continue

        # Extract the name between the 'FF FF FF FF' pattern and the ".gsc" string
        save_name_bytes = data[start_index + 4:gsc_index]  # Exclude the pattern itself
        save_name = save_name_bytes.decode('latin-1', errors='ignore').strip()

        # Extract zlib-compressed data that starts after the .gsc string
        zlib_data_start = gsc_index + len(search_string) + 1  # +1 to account for null terminator
        try:
            decompressed_data = zlib.decompress(data[zlib_data_start:])
        except zlib.error as e:
            try:
                decompressor = zlib.decompressobj()
                decompressed_data = decompressor.decompress(data[zlib_data_start:], 200000)
            except zlib.error as e:
                print(f"Decompression failed for {file_path} at {zlib_data_start}: {e}")
                offset = gsc_index + len(search_string)
                continue

        # Save the decompressed data to a file with the save name
        save_file_path = f"{save_name}.gsc"
        # Extract the directory part of the path (without the file name)
        dir = os.path.dirname(save_file_path)
        directory = 'Dumped_GSCs/' + dir
        # Create the directories if they don't already exist
        os.makedirs(directory, exist_ok=True)
        with open('./Dumped_GSCs/' + save_file_path, 'wb') as out_file:
            out_file.write(decompressed_data)

        print(f"Decompressed data saved to {save_file_path}")
        offset = gsc_index + len(search_string)

def search_directory(directory):
    """
    Traverse the given directory and process files with .ff and .ffd extensions.
    """
    for root, _, files in os.walk(directory):
        for file in files:
            if file.lower().endswith(('.ffm', '.ffd', '.ffdm')):
                file_path = os.path.join(root, file)
                print(f"Processing file: {file_path}")
                process_file(file_path)


if __name__ == '__main__':
    # Replace with your directory path
    directory_to_search = r'C:\Call of Duty MW3 2011 ffdm files dir\'  
    search_directory(directory_to_search)
