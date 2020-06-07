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

#include "System.hxx"
#include "CartTVBoy.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CartridgeTVBoy::CartridgeTVBoy(const ByteBuffer& image, size_t size,
                         const string& md5, const Settings& settings)
  : CartridgeEnhanced(image, size, 512_KB, md5, settings)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool CartridgeTVBoy::checkSwitchBank(uInt16 address, uInt8)
{
  // Switch banks if necessary
  if((address & ADDR_MASK) >= 0x1800 && (address & ADDR_MASK) <= 0x187F)
  {
    bank(address & (romBankCount() - 1));
    return true;
  }
  return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool CartridgeTVBoy::bank(uInt16 bank, uInt16)
{
  if(myBankingDisabled) return false;

  bool banked = CartridgeEnhanced::bank(bank);

  // Any bankswitching locks further bankswitching, we check for bank 0
  // to avoid locking on cart init.
  if (banked && bank != 0)
    myBankingDisabled = true;

  return banked;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool CartridgeTVBoy::save(Serializer& out) const
{
  CartridgeEnhanced::save(out);
  try
  {
    out.putBool(myBankingDisabled);
  }
  catch(...)
  {
    cerr << "ERROR: CartridgeTVBoy::save" << endl;
    return false;
  }

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool CartridgeTVBoy::load(Serializer& in)
{
  CartridgeEnhanced::load(in);
  try
  {
    myBankingDisabled = in.getBool();
  }
  catch(...)
  {
    cerr << "ERROR: CartridgeTVBoy::load" << endl;
    return false;
  }

  return true;
}
