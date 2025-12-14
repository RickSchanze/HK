# GDB initialization file for HKEngine
# Load Python formatters from Debug directory
python
import sys
import os
# Get the directory where this .gdbinit file is located
script_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, script_dir)
import gdb_formatters
end

