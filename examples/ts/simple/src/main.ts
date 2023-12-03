import {Person} from "./person.np.js"

console.log("a simple program demonstrating conversion between NanoPack data & TypeScript class.")

const person = new Person(
    "John",
    null,
    "Doe",
    40,
    null,
)

const bytes = person.bytes()
console.log("raw bytes: ", [...bytes])
console.log("")
console.log("total bytes:", bytes.byteLength)

const {result: person1} = Person.fromBytes(bytes)!
console.log("first name:", person1.firstName)
console.log("last name:", person1.lastName)
console.log("age:", person1.age)
if (!person1.middleName) {
    console.log("this person does not have a middle name.")
}
