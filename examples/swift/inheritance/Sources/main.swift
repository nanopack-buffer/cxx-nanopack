print("NanoPack supports inheritance!")
print("In this example, Text inherits Widget, so it inherits all the declared fields of Widgets.")

let widget_: Widget = Text(id: 123, content: "hello world")
let data = widget_.data()!

let message = makeNanoPackMessage(from: data, typeID: Text_typeID)!
let widget = message as! Widget

print("Read \(data.count) bytes.")
print("ID of text (inherited from Widget): \(widget.id)")

let text = widget as! Text
print("Content of text (declared): \(text.content)")
