#!/usr/bin/env python
"""
GDB formatters for HKEngine types
"""

import gdb


class FNamePrinter:
    """Pretty printer for FName"""
    
    def __init__(self, val):
        self.val = val
    
    def to_string(self):
        try:
            id_val = int(self.val['ID'])
            if id_val == 0:
                return "FName(Invalid)"
            
            # Try to get the string value by calling GetString()
            # Note: This requires the object to be in a valid state
            try:
                # Use gdb to call the method
                frame = gdb.selected_frame()
                if frame:
                    # Create a temporary variable to hold the result
                    result = gdb.parse_and_eval(f'((FName*){self.val.address})->GetString().GetStdString()')
                    str_val = result.string()
                    return f"FName(ID={id_val}, String=\"{str_val}\")"
            except:
                pass
            
            return f"FName(ID={id_val})"
        except:
            return "FName(?)"
    
    def children(self):
        try:
            id_val = int(self.val['ID'])
            yield "ID", id_val
            
            # Try to get string
            try:
                frame = gdb.selected_frame()
                if frame:
                    result = gdb.parse_and_eval(f'((FName*){self.val.address})->GetString().GetStdString()')
                    str_val = result.string()
                    yield "String", f'"{str_val}"'
                else:
                    yield "String", '"<unavailable>"'
            except:
                yield "String", '"<unavailable>"'
        except:
            pass


def register_printers():
    """Register all printers with GDB"""
    gdb.pretty_printers.append(lambda val: FNamePrinter(val) if val.type.tag == 'FName' else None)


# Auto-register when imported
register_printers()

