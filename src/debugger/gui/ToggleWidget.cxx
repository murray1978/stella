//============================================================================
//
//   SSSS    tt          lll  lll
//  SS  SS   tt           ll   ll
//  SS     tttttt  eeee   ll   ll   aaaa
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2020 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//============================================================================

#include "OSystem.hxx"
#include "StellaKeys.hxx"
#include "Widget.hxx"
#include "ToggleWidget.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ToggleWidget::ToggleWidget(GuiObject* boss, const GUI::Font& font,
                           int x, int y, int cols, int rows,
                           int clicksToChange)
  : Widget(boss, font, x, y, 16, 16),
    CommandSender(boss),
    _rows(rows),
    _cols(cols),
    _currentRow(0),
    _currentCol(0),
    _rowHeight(0),
    _colWidth(0),
    _selectedItem(0),
    _clicksToChange(clicksToChange),
    _editable(true)
{
  _flags = Widget::FLAG_ENABLED | Widget::FLAG_CLEARBG | Widget::FLAG_RETAIN_FOCUS |
           Widget::FLAG_WANTS_RAWDATA;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ToggleWidget::handleMouseEntered()
{
  setFlags(Widget::FLAG_HILITED);
  setDirty();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ToggleWidget::handleMouseLeft()
{
  clearFlags(Widget::FLAG_HILITED);
  setDirty();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ToggleWidget::handleMouseDown(int x, int y, MouseButton b, int clickCount)
{
  if (!isEnabled())
    return;

  // First check whether the selection changed
  int newSelectedItem;
  newSelectedItem = findItem(x, y);
  if (newSelectedItem > int(_stateList.size()) - 1)
    newSelectedItem = -1;

  if (_selectedItem != newSelectedItem)
  {
    _selectedItem = newSelectedItem;
    _currentRow = _selectedItem / _cols;
    _currentCol = _selectedItem - (_currentRow * _cols);
    setDirty();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ToggleWidget::handleMouseUp(int x, int y, MouseButton b, int clickCount)
{
  if (!isEnabled() || !_editable)
    return;

  // If this was a double click and the mouse is still over the selected item,
  // send the double click command
  if (clickCount == _clicksToChange && (_selectedItem == findItem(x, y)))
  {
    _stateList[_selectedItem] = !_stateList[_selectedItem];
    _changedList[_selectedItem] = !_changedList[_selectedItem];
    sendCommand(ToggleWidget::kItemDataChangedCmd, _selectedItem, _id);
    setDirty();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int ToggleWidget::findItem(int x, int y)
{
  int row = (y - 1) / _rowHeight;
  if(row >= _rows) row = _rows - 1;

  int col = x / _colWidth;
  if(col >= _cols) col = _cols - 1;

  return row * _cols + col;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool ToggleWidget::handleKeyDown(StellaKey key, StellaMod mod)
{
  // Ignore all mod keys
  if(StellaModTest::isControl(mod) || StellaModTest::isAlt(mod))
    return true;

  bool handled = true;
  bool dirty = false, toggle = false;

  switch(key)
  {
    case KBDK_RETURN:
    case KBDK_KP_ENTER:
      if (_currentRow >= 0 && _currentCol >= 0)
      {
        dirty = true;
        toggle = true;
      }
      break;

    case KBDK_UP:
      if (_currentRow > 0)
      {
        _currentRow--;
        dirty = true;
      }
      break;

    case KBDK_DOWN:
      if (_currentRow < int(_rows) - 1)
      {
        _currentRow++;
        dirty = true;
      }
      break;

    case KBDK_LEFT:
      if (_currentCol > 0)
      {
        _currentCol--;
        dirty = true;
      }
      break;

    case KBDK_RIGHT:
      if (_currentCol < int(_cols) - 1)
      {
        _currentCol++;
        dirty = true;
      }
      break;

    case KBDK_PAGEUP:
      if (_currentRow > 0)
      {
        _currentRow = 0;
        dirty = true;
      }
      break;

    case KBDK_PAGEDOWN:
      if (_currentRow < int(_rows) - 1)
      {
        _currentRow = _rows - 1;
        dirty = true;
      }
      break;

    case KBDK_HOME:
      if (_currentCol > 0)
      {
        _currentCol = 0;
        dirty = true;
      }
      break;

    case KBDK_END:
      if (_currentCol < int(_cols) - 1)
      {
        _currentCol = _cols - 1;
        dirty = true;
      }
      break;

    default:
      handled = false;
  }

  if (dirty)
  {
    _selectedItem = _currentRow*_cols + _currentCol;

    if(toggle && _editable)
    {
      _stateList[_selectedItem] = !_stateList[_selectedItem];
      _changedList[_selectedItem] = !_changedList[_selectedItem];
      sendCommand(ToggleWidget::kItemDataChangedCmd, _selectedItem, _id);
    }

    setDirty();
  }

  return handled;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ToggleWidget::handleCommand(CommandSender* sender, int cmd,
                                 int data, int id)
{
  if (cmd == GuiObject::kSetPositionCmd)
  {
    if (_selectedItem != data)
    {
      _selectedItem = data;
      setDirty();
    }
  }
}
