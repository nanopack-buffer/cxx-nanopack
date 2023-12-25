// AUTOMATICALLY GENERATED BY NANOPACK. DO NOT MODIFY BY HAND.

import Foundation
import NanoPack

let Widget_typeID: TypeID = 1

class Widget: NanoPackMessage {
  let id: Int32

  init(id: Int32) {
    self.id = id
  }

  static func from(data: Data) -> Widget? {
    switch data.readTypeID() {
    case Widget_typeID: return Widget(data: data)
    case Text_typeID: return Text(data: data)
    default: return nil
    }
  }

  required init?(data: Data) {
    guard data.readTypeID() == Widget_typeID else {
      return nil
    }

    var ptr = 8

    let id: Int32 = data.read(at: ptr)
    ptr += 4

    self.id = id
  }

  func data() -> Data? {
    var data = Data()
    data.reserveCapacity(8)

    withUnsafeBytes(of: Int32(Widget_typeID)) {
      data.append(contentsOf: $0)
    }

    data.append([0], count: 4)

    data.write(size: 4, ofField: 0)
    data.append(int: id)

    return data
  }
}
