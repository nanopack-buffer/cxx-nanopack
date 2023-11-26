import Foundation
import NanoPack

func makeNanoPackMessage(from data: Data, typeID: TypeID) -> NanoPackMessage? {
  switch typeID {
  case 1: return Widget(data: data)
  case 2: return Text(data: data)
  default: return nil
  }
}
