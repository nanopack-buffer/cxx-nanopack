// The Swift Programming Language
// https://docs.swift.org/swift-book

import NanoPack

let person = Person(firstName: "John", lastName: "Doe", age: 40)

let data = person.data()!
print("raw bytes: ", terminator: "")
for b in data {
    print("\(b)", terminator: " ")
}
print("")
print("total bytes:", data.count)

print("========================")

let person2 = Person(data: data)!
print("first name:", person2.firstName)
print("last name:", person2.lastName)
print("age:", person2.age)
