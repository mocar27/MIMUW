public class Main {

    public static void main(String[] args) {
//        Expression e = new Multiplication(Sum.create(new Constant(2), new Constant(1)), new Variable("x"));
        Expression e = new Multiplication(new Variable("x"), new Sum(new Sum(new Sin(new Variable("x")), new Constant(3)), new Variable("x")));
//        Expression e = new Multiplication(new Constant(4), new Sum(new Constant(2), new Variable("x")));
//        Expression e = Sum.create(Constant.create(1), Constant.create(0));
        System.out.println(e);
    }
}
