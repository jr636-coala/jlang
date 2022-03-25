#entry(main);

const ns = ::{
    fn private(a, b: i8, c: string, d: u16) {
        #output(a,b,c,d);
    }

    ::public = fn (name) {
        #output("Hello " + name);
    }
}

fn main():i64 {
    ns::public("Jay");
    let a : Vec2(1,2);
    let b = Vec2(3,4);
    #output(a + b);
    return 0;
}

pod Vec2 {
    ::(x, y) {
        ::x = x;
        this.y = y;
    }

    ::+::(b:Vec2): Vec2 {
        return {x + b.x, y + b.y};
    }

    ::-::(b:Vec2): Vec2 {
        return {x - b.x, y - b.y};
    }

    ::+=::(b:Vec2):Vec2 {
        this.x += b.x;
        this.y += b.y;
        return ::; // Or 'this'
    }

    ::(): string {
        return string(x) + ", " + string(y);
    }

    ::x : f64;
    ::y : f64;
}