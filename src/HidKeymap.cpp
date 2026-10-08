#include "HidKeymap.h"

char hidToAscii(uint8_t hid, bool shift, bool capsLock)
{
    // A-Z (0x04 - 0x1D)
    if (hid >= 0x04 && hid <= 0x1D)
    {
        bool uppercase = (shift ^ capsLock);
        return uppercase ? ('A' + hid - 0x04) : ('a' + hid - 0x04);
    }

    // 1-9 (0x1E - 0x26)
    if (hid >= 0x1E && hid <= 0x26)
    {
        if (shift)
        {
            const char shiftNums[] = "!@#$%^&*(";
            return shiftNums[hid - 0x1E];
        }
        return '1' + (hid - 0x1E);
    }

    // 0 (0x27)
    if (hid == 0x27)
        return shift ? ')' : '0';

    // Standard Punctuation and Symbols
    switch (hid)
    {
        case 0x2C: return ' ';                     // Space
        case 0x28: return '\n';                    // Enter
        case 0x2A: return '\b';                    // Backspace
        case 0x2B: return '\t';                    // Tab

        case 0x2D: return shift ? '_' : '-';
        case 0x2E: return shift ? '+' : '=';
        case 0x2F: return shift ? '{' : '[';
        case 0x30: return shift ? '}' : ']';
        case 0x31: return shift ? '|' : '\\';
        case 0x33: return shift ? ':' : ';';
        case 0x34: return shift ? '"' : '\'';
        case 0x35: return shift ? '~' : '`';
        case 0x36: return shift ? '<' : ',';
        case 0x37: return shift ? '>' : '.';
        case 0x38: return shift ? '?' : '/';

        // Keypad keys
        case 0x54: return '/';
        case 0x55: return '*';
        case 0x56: return '-';
        case 0x57: return '+';
        case 0x58: return '\n';                    // Keypad Enter
        case 0x59: case 0x5A: case 0x5B:
        case 0x5C: case 0x5D: case 0x5E:
        case 0x5F: case 0x60: case 0x61:
            return '1' + (hid - 0x59);             // Keypad 1-9
        case 0x62: return '0';                     // Keypad 0
        case 0x63: return '.';                     // Keypad .

        default:
            return 0;
    }
}

const char* hidKeyToString(uint8_t hid)
{
    switch (hid)
    {
        case 0x28: return "Enter";
        case 0x29: return "Escape";
        case 0x2A: return "Backspace";
        case 0x2B: return "Tab";
        case 0x2C: return "Space";
        case 0x39: return "CapsLock";

        // Function keys F1 - F12
        case 0x3A: return "F1";
        case 0x3B: return "F2";
        case 0x3C: return "F3";
        case 0x3D: return "F4";
        case 0x3E: return "F5";
        case 0x3F: return "F6";
        case 0x40: return "F7";
        case 0x41: return "F8";
        case 0x42: return "F9";
        case 0x43: return "F10";
        case 0x44: return "F11";
        case 0x45: return "F12";

        // Navigation / Control
        case 0x46: return "PrintScreen";
        case 0x47: return "ScrollLock";
        case 0x48: return "Pause";
        case 0x49: return "Insert";
        case 0x4A: return "Home";
        case 0x4B: return "PageUp";
        case 0x4C: return "Delete";
        case 0x4D: return "End";
        case 0x4E: return "PageDown";

        // Arrows
        case 0x4F: return "Right";
        case 0x50: return "Left";
        case 0x51: return "Down";
        case 0x52: return "Up";

        case 0x53: return "NumLock";

        default:
            return nullptr;
    }
}

String hidFormatKeyCombo(uint8_t modifiers, uint8_t hidKey, bool capsLock)
{
    bool hasCtrl  = (modifiers & 0x11) != 0;
    bool hasShift = (modifiers & 0x22) != 0;
    bool hasAlt   = (modifiers & 0x44) != 0;
    bool hasGUI   = (modifiers & 0x88) != 0;

    const char* specialName = hidKeyToString(hidKey);

    // If Ctrl, Alt, GUI are active, or Shift with letters or special keys (excluding Space/Enter)
    if (hasCtrl || hasAlt || hasGUI || (hasShift && (specialName || (hidKey >= 0x04 && hidKey <= 0x1D))))
    {
        // Don't format normal Shift+Enter or Shift+Space as combo unless other modifiers present
        if (!hasCtrl && !hasAlt && !hasGUI && (hidKey == 0x2C /* Space */ || hidKey == 0x28 /* Enter */))
        {
            return "";
        }

        String combo = "";
        if (hasCtrl)  combo += "Ctrl+";
        if (hasAlt)   combo += "Alt+";
        if (hasGUI)   combo += "GUI+";
        if (hasShift) combo += "Shift+";

        if (specialName)
        {
            combo += specialName;
        }
        else if (hidKey >= 0x04 && hidKey <= 0x1D)
        {
            combo += (char)('A' + hidKey - 0x04);
        }
        else if (hidKey >= 0x1E && hidKey <= 0x26)
        {
            combo += (char)('1' + hidKey - 0x1E);
        }
        else if (hidKey == 0x27)
        {
            combo += '0';
        }
        else
        {
            char c = hidToAscii(hidKey, false, false);
            if (c) combo += c;
            else combo += String("0x") + String(hidKey, HEX);
        }

        return combo;
    }

    return "";
}
