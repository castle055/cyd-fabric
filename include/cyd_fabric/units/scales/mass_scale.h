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

#ifndef CYD_FABRIC_MASS_SCALE_H
#define CYD_FABRIC_MASS_SCALE_H

namespace cyd::fabric::units {
  SCALE(mass, {
    UNIT(grams, "g", 1,1)
    
    //! Metric
    UNIT(decigrams, "dg", 1,10)
    UNIT(centigrams, "cg", 1,100)
    UNIT(milligrams, "mg", 1,1000)
    UNIT(micrograms, "ug", 1,1000000)
    UNIT(nanograms, "ng", 1,1000000000)
    
    UNIT(decagrams, "Dg", 10,1)
    UNIT(hectograms, "hg", 100,1)
    UNIT(kilograms, "kg", 1000, 1)
    UNIT(ton, "T", 1000000, 1)
    
  })
}

#endif //CYD_FABRIC_MASS_SCALE_H
