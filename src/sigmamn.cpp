// ***************************************************************** -*- C++ -*-
/*
 * Copyright (C) 2004 Andreas Huggel <ahuggel@gmx.net>
 * 
 * This program is part of the Exiv2 distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
/*
  File:      sigmamn.cpp
  Version:   $Name:  $ $Revision: 1.4 $
  Author(s): Andreas Huggel (ahu) <ahuggel@gmx.net>
  History:   02-Apr-04, ahu: created
  Credits:   Sigma and Foveon MakerNote implemented according to the specification
             in "SIGMA and FOVEON EXIF MakerNote Documentation" by Foveon.
             <http://www.x3f.info/technotes/FileDocs/MakerNoteDoc.html>
 */
// *****************************************************************************
#include "rcsid.hpp"
EXIV2_RCSID("@(#) $Name:  $ $Revision: 1.4 $ $RCSfile: sigmamn.cpp,v $")

// *****************************************************************************
// included header files
#include "types.hpp"
#include "sigmamn.hpp"
#include "makernote.hpp"
#include "value.hpp"

// + standard includes
#include <string>
#include <sstream>
#include <iomanip>

// Define DEBUG_MAKERNOTE to output debug information to std::cerr
#undef DEBUG_MAKERNOTE

// *****************************************************************************
// class member definitions
namespace Exiv2 {

    const SigmaMakerNote::RegisterMakerNote SigmaMakerNote::register_;

    // Sigma (Foveon) MakerNote Tag Info
    static const MakerNote::MnTagInfo sigmaMnTagInfo[] = {
        MakerNote::MnTagInfo(0x0002, "SerialNumber", "Camera serial number"),
        MakerNote::MnTagInfo(0x0003, "DriveMode", "Drive Mode"),
        MakerNote::MnTagInfo(0x0004, "ResolutionMode", "Resolution Mode"),
        MakerNote::MnTagInfo(0x0005, "AutofocusMode", "Autofocus mode"),
        MakerNote::MnTagInfo(0x0006, "FocusSetting", "Focus setting"),
        MakerNote::MnTagInfo(0x0007, "WhiteBalance", "White balance"),
        MakerNote::MnTagInfo(0x0008, "ExposureMode", "Exposure mode"),
        MakerNote::MnTagInfo(0x0009, "MeteringMode", "Metering mode"),
        MakerNote::MnTagInfo(0x000a, "LensRange", "Lens focal length range"),
        MakerNote::MnTagInfo(0x000b, "ColorSpace", "Color space"),
        MakerNote::MnTagInfo(0x000c, "Exposure", "Exposure"),
        MakerNote::MnTagInfo(0x000d, "Contrast", "Contrast"),
        MakerNote::MnTagInfo(0x000e, "Shadow", "Shadow"),
        MakerNote::MnTagInfo(0x000f, "Highlight", "Highlight"),
        MakerNote::MnTagInfo(0x0010, "Saturation", "Saturation"),
        MakerNote::MnTagInfo(0x0011, "Sharpness", "Sharpness"),
        MakerNote::MnTagInfo(0x0012, "FillLight", "X3 Fill light"),
        MakerNote::MnTagInfo(0x0014, "ColorAdjustment", "Color adjustment"),
        MakerNote::MnTagInfo(0x0015, "AdjustmentMode", "Adjustment mode"),
        MakerNote::MnTagInfo(0x0016, "Quality", "Quality"),
        MakerNote::MnTagInfo(0x0017, "Firmware", "Firmware"),
        MakerNote::MnTagInfo(0x0018, "Software", "Software"),
        MakerNote::MnTagInfo(0x0019, "AutoBracket", "Auto bracket"),
        // End of list marker
        MakerNote::MnTagInfo(0xffff, "(UnknownSigmaMakerNoteTag)", "Unknown SigmaMakerNote tag")
    };

    SigmaMakerNote::SigmaMakerNote(bool alloc)
        : IfdMakerNote(sigmaMnTagInfo, alloc), sectionName_("Sigma")
    {
        // My one and only Sigma sample has two undocumented extra bytes 
        // (0x01, 0x00) between the ID string and the start of the
        // Makernote IFD. Adding them to the ID string is a hack...
        prefix_ = std::string("SIGMA\0\0\0\x1\0", 10);
    }

    MakerNote* SigmaMakerNote::clone(bool alloc) const 
    {
        return createSigmaMakerNote(alloc); 
    }

    std::ostream& SigmaMakerNote::printTag(std::ostream& os, 
                                          uint16 tag, 
                                          const Value& value) const
    {
        switch (tag) {
        case 0x000c: // fallthrough
        case 0x000d: // fallthrough
        case 0x000e: // fallthrough
        case 0x000f: // fallthrough
        case 0x0010: // fallthrough
        case 0x0011: // fallthrough
        case 0x0012: // fallthrough
        case 0x0014: // fallthrough
        case 0x0016: printStripLabel(os, value); break;
        case 0x0008: print0x0008(os, value); break;
        case 0x0009: print0x0009(os, value); break;
        default:
            // All other tags (known or unknown) go here
            os << value;
            break;
        }
        return os;
    }

    std::ostream& SigmaMakerNote::printStripLabel(std::ostream& os,
                                                  const Value& value)
    {
        std::string v = value.toString();
        std::string::size_type pos = v.find(':');
        if (pos != std::string::npos) {
            if (v[pos + 1] == ' ') ++pos;
            v = v.substr(pos + 1);
        }
        return os << v;
    }

    std::ostream& SigmaMakerNote::print0x0008(std::ostream& os,
                                              const Value& value)
    {
        switch (value.toString()[0]) {
        case 'P': os << "Program"; break;
        case 'A': os << "Aperture priority"; break;
        case 'S': os << "Shutter priority"; break;
        case 'M': os << "Manual"; break;
        default: os << "(" << value << ")"; break;
        }
        return os;
    }

    std::ostream& SigmaMakerNote::print0x0009(std::ostream& os,
                                              const Value& value)
    {
        switch (value.toString()[0]) {
        case 'A': os << "Average"; break;
        case 'C': os << "Center"; break;
        case '8': os << "8-Segment"; break;
        default: os << "(" << value << ")"; break;
        }
        return os;
    }

// *****************************************************************************
// free functions

    MakerNote* createSigmaMakerNote(bool alloc)
    {
        return new SigmaMakerNote(alloc);         
    }

}                                       // namespace Exiv2
