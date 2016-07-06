//Author: demon1991@gmail.com

#include "slave/logevent.h"

namespace fdemo{
namespace slave{


void RotateEvent::unpack(const ByteArray& bytes) {
    position = bytes.getFixed64();   
    filename = bytes.getLeft();
}

} // namespace slave
} //namespace fdemo
