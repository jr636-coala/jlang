const x = ::{
    ::STATIC = fn () {
        #output("Function call ns");
        xyz();
    };

    ::x = ::{
        ::static2 = fn() {
            #output("Function call nested ns");
        }
    };

    fn xyz() {
        #output("Private ns member");
    };
};

fn a() {
    //#output(b);
    #output("Function call a");
};

fn main() {
    #output("Hello, World!");
    #output(3 * 4 + 2 * 3);
    x::STATIC();
    x::xyz(); // Should not work as xyz is private within x
    x::x::static2();

    let y = x;

    /* Multiline comment */

    #output(#loc());

    y = 10;

    fn abc(x : i8) {
        y += x;
        return y;
    };
    a();
    abc(467);
    #output(y);
    return abc(9999999);
};

#entry(main);