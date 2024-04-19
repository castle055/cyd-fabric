// Copyright (c) 2024, Victor Castillo, All rights reserved.

#ifndef CYD_FABRIC_SPEED_SCALE_H
#define CYD_FABRIC_SPEED_SCALE_H

namespace cyd::fabric::units {
  namespace speed {
    using scale = frac<distance::scale, time::scale>;
    template <typename Q> concept quantity = Quantity<Q, scale>;
    
    using m_s = frac<distance::meters,time::seconds>;
  }
}

#endif //CYD_FABRIC_SPEED_SCALE_H
