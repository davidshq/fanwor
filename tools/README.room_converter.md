# Fanwor Room Converter

The Room Converter is a tool for Fanwor that converts room files between binary format (used by the game) and text format (easier to edit).

## Building

The room converter is located in the `tools/` directory of the Fanwor project.

To build the tool, simply run:

```bash
cd tools
make
```

This will create the `room_converter` executable.

## Usage

The room converter can be used in several ways:

1. Convert a single binary room file to text:
   ```bash
   ./room_converter -b rooms/room512 room512.txt
   ```

2. Convert a single text room file to binary:
   ```bash
   ./room_converter -t room512.txt rooms/room512
   ```

3. Convert all binary room files in a directory to text:
   ```bash
   ./room_converter -d rooms/ rooms_text/
   ```

4. Convert all text room files in a directory to binary:
   ```bash
   ./room_converter -r rooms_text/ rooms/
   ```

## File Format

### Binary Format

The binary format is used by the game and consists of the following structure:

1. **Header** (44 bytes):
   - Magic Number (4 bytes): 'FAWO'
   - Version (2 bytes): e.g., 0x0250
   - Title (32 bytes): Name of the level (null-terminated string)
   - Object Count (2 bytes): Number of objects in the room
   - Room Width (2 bytes): Width of the room in tiles
   - Room Height (2 bytes): Height of the room in tiles

2. **Room Data** (Width × Height bytes):
   - Each byte represents a tile in the room

3. **Field Data** (Width × Height bytes):
   - Additional data for each tile position

4. **Objects** (8 bytes per object):
   - Type (1 byte): 1=Enemy, 2=Door, 3=People, 4=Item
   - Art (1 byte): Art/sprite index
   - X Position (1 byte): X coordinate
   - Y Position (1 byte): Y coordinate
   - Width (1 byte): Width of the object
   - Height (1 byte): Height of the object
   - Special 1 (1 byte): Special parameter 1
   - Special 2 (1 byte): Special parameter 2

### Text Format

The text format is designed to be human-readable and editable. It consists of:

1. **Header Information**:
   ```
   # Fanwor Room File: rooms/room512
   version: 0x0250
   title: Room 512
   width: 16
   height: 11
   object_count: 2
   endian: little
   ```

2. **Room Data**:
   ```
   [Room Data]
   0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
   0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
   ...
   ```

3. **Field Data**:
   ```
   [Field Data]
   0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
   0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
   ...
   ```

4. **Objects**:
   ```
   [Objects]
   # type, art, xpos, ypos, xl, yl, spec1, spec2, description
   1, 3, 5, 6, 1, 1, 0, 0  # Enemy
   2, 1, 10, 8, 1, 1, 12, 0  # Door
   ```

## Notes

- When converting from binary to text, the converter adds ".txt" extension to the output filenames
- When converting from text to binary, the converter strips the ".txt" extension if present
- The binary room files must have a valid magic number ('FAWO') to be recognized

## Text File Format

The text files use a simple format with sections for different parts of the room data:

```
# Fanwor Room File: rooms/room512
version: 0x0250
title: 
width: 16
height: 11
object_count: 3
endian: little

[Room Data]
 56  56  56  56  56  56  56  13  13  56  56  56  56  56  56  56
 56  56  56  56  56  56  56  13  13  56  56  56  56  56  56  56
 56  56  13  13  13  13  13  13  13  13  13  13  13  13  56  56
 56  56  13  13  13  13  13  13  13  13  13  13  13  13  56  56
 56  56  13  13  13  13  13  64  64  13  13  13  13  13  56  56
 13  13  13  13  13  13  64  64  64  64  13  13  13  13  13  13
 56  56  13  13  13  13  13  64  64  13  13  13  13  13  56  56
 56  56  13  13  13  13  13  13  13  13  13  13  13  13  56  56
 56  56  13  13  13  13  13  13  13  13  13  13  13  13  56  56
 56  56  56  56  56  56  56  13  13  56  56  56  56  56  56  56
 56  56  56  56  56  56  56  13  13  56  56  56  56  56  56  56

[Field Data]
  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0

[Objects]
# type, art, xpos, ypos, xl, yl, spec1, spec2, description
1, 26, 6, 7, 0, 0, 0, 0  # Enemy
1, 26, 10, 6, 0, 0, 0, 0  # Enemy
1, 26, 3, 3, 0, 0, 0, 0  # Enemy
```

## Binary File Format

The binary files contain the following structure:

1. A header (LEVEL_HEADER) with:
   - Magic number ('FAWO' / 0x4641574F)
   - Version information (2 bytes)
   - Title (32 bytes)
   - Object count (2 bytes)
   - Room width (2 bytes)
   - Room height (2 bytes)

2. Room tile data (one byte per tile, width*height bytes)

3. Field data (one byte per tile, width*height bytes)

4. Object entries (LEVEL_ENTRY structures, 8 bytes each):
   - entry_type (1 byte): 1=Enemy, 2=Door, 3=People, 4=Item
   - art (1 byte): Art/sprite index
   - xpos, ypos (1 byte each): Position in the room
   - xl, yl (1 byte each): Used for doors (exit coordinates)
   - spec1, spec2 (1 byte each): Special values (for doors: room destination)

## Ensuring Binary Compatibility

The converter uses several techniques to ensure the converted files are binary-identical to the originals:

1. When converting text to binary, it uses the original binary file as a template, preserving any unknown data or format details.

2. It handles endianness differences between platforms, detecting and preserving the original byte order.

3. The room and field data are carefully parsed and written back in the exact same format and order.

4. The object entries are reconstructed with the same memory layout as the original.

This approach ensures that the converted files can be used by the game on any platform, including the original Atari ST.

## Notes

- The original game expects binary files, so any text files must be converted back to binary before use.
- The converter handles byte order differences between platforms (big-endian vs little-endian).
- When editing room files, be careful to maintain the correct format to avoid corruption.
- The original room files use a 16x11 grid for most rooms.
- Room indices are in the format 'roomXYZ' where X is the room number (level), and Y,Z are coordinates.

## Known Issues

- The field data parsing could be improved to better handle multiline spacing.
- The text file parser relies on specific formatting - be careful when editing manually.

## File Types

- Rooms starting with numbers (512, 513, etc.) are the main game levels.
- Rooms with letter prefixes (like 0c0, 0d1, etc.) are special areas or dungeons.

For more details on the file format, see the original game's source code in `level.h` and `fwdisk.c`. 