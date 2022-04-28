const file = #module("file.jlm");

// The syntax highlighting is horrible for this so am wrapping it
fn print(str : string) { #output(str); }

fn whileTest() {
    let i = 10;
    while(i) {
        print(i);
        i -= 1;
    }
}

fn ifTest() {
    if (FALSE) {
        print("It is true");
    }
    else {
        print("It is false");
    }
}

fn fileTest() {
    // File testing
    let f = file::open("bin/test.txt");
    file::fwrite(f, "Hello, World!\n");
    file::close(f);
}

fn arrayTest() {
    let arr = ["Something", "Something 2", 3];
    print(arr[1]);
    //print(arr[1]);
}

pod obj {
}

const TRUE = 1;
const FALSE = 0;

fn main() {
    print("Hello, World!");

    /*
        Multiline comment
    */

    //print("Length of string " + file::mstring::len("Hello"));
    //print("I am currently at : " + #loc());

    let y = 10;
    fn abc(x : i8) {
        y += x;
        return y;
    }

    //whileTest();
    //ifTest();
    //fileTest();
    arrayTest();

    return abc(9999999);
}

#entry(main);