#!/usr/bin/env python
"""
LLDB formatters for HKEngine types
"""

import lldb


def __lldb_init_module(debugger, internal_dict):
    """Register formatters when LLDB loads this module"""
    debugger.HandleCommand(
        'type summary add -F lldb_formatters.fname_summary FName'
    )
    debugger.HandleCommand(
        'type synthetic add -l lldb_formatters.FNameSyntheticProvider -x "^FName$"'
    )


def fname_summary(value, internal_dict):
    """Summary formatter for FName"""
    try:
        id_val = value.GetChildMemberWithName('ID').GetValueAsUnsigned()
        if id_val == 0:
            return "FName(Invalid)"
        
        # Try to get the string value
        # Note: This requires calling GetString() which may not work in all contexts
        # We'll show the ID and indicate if we can get the string
        try:
            # Attempt to evaluate GetString().GetStdString()
            # This may fail if the debugger can't call methods
            frame = value.GetFrame()
            if frame:
                result = frame.EvaluateExpression('((FName*)' + str(value.GetAddress()) + ')->GetString().GetStdString()')
                if result.GetError().Success():
                    str_val = result.GetSummary()
                    if str_val:
                        return f"FName(ID={id_val}, String={str_val})"
        except:
            pass
        
        return f"FName(ID={id_val})"
    except:
        return "FName(?)"


class FNameSyntheticProvider:
    """Synthetic children provider for FName"""
    
    def __init__(self, valobj, internal_dict):
        self.valobj = valobj
        self.update()
    
    def num_children(self):
        return 2
    
    def get_child_index(self, name):
        if name == "ID":
            return 0
        elif name == "String":
            return 1
        return None
    
    def get_child_at_index(self, index):
        try:
            if index == 0:
                return self.valobj.GetChildMemberWithName('ID')
            elif index == 1:
                # Try to get the string representation
                id_val = self.valobj.GetChildMemberWithName('ID').GetValueAsUnsigned()
                if id_val == 0:
                    return self.valobj.CreateValueFromExpression("String", '"Invalid"')
                
                # Try to evaluate GetString().GetStdString()
                frame = self.valobj.GetFrame()
                if frame:
                    result = frame.EvaluateExpression('((FName*)' + str(self.valobj.GetAddress()) + ')->GetString().GetStdString()')
                    if result.GetError().Success():
                        str_val = result.GetSummary()
                        if str_val:
                            return self.valobj.CreateValueFromExpression("String", str_val)
                
                return self.valobj.CreateValueFromExpression("String", '"<unavailable>"')
        except:
            pass
        return None
    
    def update(self):
        pass
    
    def has_children(self):
        return True

