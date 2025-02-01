import sys
import zlib

with open(sys.argv[1], 'rb') as compressed_data:
    compressed_data = compressed_data.read()
    unzipped = zlib.decompress(compressed_data)

    if len(sys.argv) > 2:
        with open(sys.argv[2], 'wb') as result:
            result.write(unzipped);
            result.close()
    else:
        path = sys.argv[1]
        filename = path[path.rfind('\\') + 1:]
        with open(filename, 'wb') as result:
            result.write(unzipped);
            result.close()
