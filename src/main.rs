extern crate quickjs_rs;

use quickjs_rs::Context;

fn main() {
    let ctx = Context::new();

    let res = ctx.eval(r#"
        const x = 5;
        JSON.stringify({
            foo: 'bar',
            date: new Date(),
            rnd: Math.random()
        });
    "#);

    println!("res: {:?}", res);

    let res = ctx.eval(r#"
        JSON.stringify({
            val: x
        });
    "#);

    println!("res: {:?}", res);
}
