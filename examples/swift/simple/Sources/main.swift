// The Swift Programming Language
// https://docs.swift.org/swift-book

import NanoPack

print("a simple program demonstrating conversion between NanoPack data & Swift struct.")

let person = Person(firstName: "John", middleName: nil, lastName: "Doe", age: 40)

let data = person.data()!
print("raw bytes: ", terminator: "")
for b in data {
    print("\(b)", terminator: " ")
}
print("")
print("total bytes:", data.count)

let person1 = Person(data: data)!
print("first name:", person1.firstName)
print("last name:", person1.lastName)
print("age:", person1.age)
if person1.middleName == nil {
    print("This person does not have a middle name.")
}
