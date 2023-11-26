// The Swift Programming Language
// https://docs.swift.org/swift-book

import Foundation

let json = "{\"firstName\":\"John\",\"middleName\":null,\"lastName\":\"Doe\",\"age\":40,\"otherFriend\":{\"firstName\":\"Steve\",\"middleName\":null,\"lastName\":\"Jobs\",\"age\":52,\"otherFriend\":null}}".data(using: .utf8)!

let person = Person(
    firstName: "John",
    middleName: nil,
    lastName: "Doe",
    age: 40,
    otherFriend: Person(
        firstName: "Steve",
        middleName: nil,
        lastName: "Jobs",
        age: 52,
        otherFriend: nil))

let data = person.data()!

let sampleSize = 100000

let a = Date().timeIntervalSince1970
for _ in 0..<sampleSize {
    let dict = try! JSONSerialization.jsonObject(with: json) as? [String: Any]
    _ = try! JSONSerialization.data(withJSONObject: dict)
}
let b = Date().timeIntervalSince1970
print("Regular JSON took: \(b - a) seconds.")

let c = Date().timeIntervalSince1970
for _ in 0..<sampleSize {
    let p = Person(data: data)!
    _ = p.data()!
}
let d = Date().timeIntervalSince1970
print("NanoPack took: \(d - c) seconds.")
