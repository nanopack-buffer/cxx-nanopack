import { Widget } from "./widget.np.js";
import { Text } from "./text.np.js";
import { makeNanoPackMessage } from "./message-factory.js";

console.log("NanoPack supports inheritance!")
console.log("In this example, Text inherits Widget, so it inherits all the declared fields of Widgets.")

const widget_: Widget = new Text(123, "hello world");
const bytes = widget_.bytes()

const { bytesRead, result: message } = makeNanoPackMessage(bytes, Text.TYPE_ID)!;
const widget = message as Widget

console.log(`Read ${bytesRead} bytes.`)
console.log(`ID of text (inherited from Widget): ${widget.id}`)

const text = widget as Text
console.log(`Content of text (declared): ${text.content}`)
