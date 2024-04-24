/*!
 ! Copyright (c) 2024, Víctor Castillo Agüero.
 ! This file is part of the Cydonia project.
 !
 ! This library is free software: you can redistribute it and/or modify
 ! it under the terms of the GNU General Public License as published by
 ! the Free Software Foundation, either version 3 of the License, or
 ! (at your option) any later version.
 !
 ! This library is distributed in the hope that it will be useful,
 ! but WITHOUT ANY WARRANTY; without even the implied warranty of
 ! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ! GNU General Public License for more details.
 !
 ! You should have received a copy of the GNU General Public License
 ! along with this library.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CYD_FABRIC_VOLUME_SCALE_H
#define CYD_FABRIC_VOLUME_SCALE_H

namespace cyd::fabric::units {
  SCALE(volume, {
    UNIT(liters, "L", 1,1)
    
    //! Metric
    UNIT(deciliters, "dL", 1,10)
    UNIT(centiliters, "cL", 1,100)
    UNIT(milliliters, "mL", 1,1000)
    UNIT(microliters, "uL", 1,1000000)
    UNIT(nanoliters, "nL", 1,1000000000)
    
    UNIT(decaliters, "DL", 10,1)
    UNIT(hectoliters, "hL", 100,1)
    UNIT(kiloliters, "kL", 1000, 1)
    UNIT(megaliters, "ML", 1000000, 1)
    
  })
}

#endif //CYD_FABRIC_VOLUME_SCALE_H
