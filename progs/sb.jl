const mstring = ::{
    ::len = fn(str: string) {
        return #c("strlen", str);
    };
};

const file = :: {
    ::open = fn (path : string) {
        return #c("fopenw", path);
    };
    ::close = fn (file : i64) {
        return #c("fclose", file);
    };
    ::fwrite = fn (file : i64, content : string) {
        return #c("fwrite", content, mstring::len(content), file);
    };
};

fn main() {
    #output("Hello, World!");

    /* Multiline comment */

    #output("Length of string " + mstring::len("Hello"));
    #output(#loc());

    let y = 10;
    fn abc(x : i8) {
        y += x;
        return y;
    };


    // File testing
    let f = file::open("test.txt");
    file::fwrite(f, "Hello World\n");
    file::close(f);


    return abc(9999999);
};

#entry(main);