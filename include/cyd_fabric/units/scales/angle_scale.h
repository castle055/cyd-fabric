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

#ifndef CYD_FABRIC_ANGLE_SCALE_H
#define CYD_FABRIC_ANGLE_SCALE_H

namespace cyd::fabric::units {
  SCALE(angle, {
    UNIT(degrees, "deg", 1,1)
    
    //! Metric
    UNIT(radians, "rad", 180*10000000000000000,31415926535893238)
  })
}

#endif //CYD_FABRIC_ANGLE_SCALE_H
