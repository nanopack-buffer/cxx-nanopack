import type { NanoPackMessage } from "nanopack";

import { Widget } from "./widget.np.js";
import { Text } from "./text.np.js";

function makeNanoPackMessage(
  bytes: Uint8Array,
  typeId: number,
): { bytesRead: number; result: NanoPackMessage } | null {
  switch (typeId) {
    case 1:
      return Widget.fromBytes(bytes);
    case 2:
      return Text.fromBytes(bytes);
    default:
      return null;
  }
}

export { makeNanoPackMessage };
