import java.io.*;

public class test_javacc_calc {

    public static void main(String[] args) throws Exception {

        System.out.println("testing javacc...");

        test_one();
        test_two();
        test_three();
    }

    private static void test_one() throws Exception {
        Calculator parser = new Calculator(new ByteArrayInputStream("3 + 4.6 - 2\n".getBytes()));
        double value = parser.Start(System.out);
        System.out.println(" + - value: " + value);
        assert(value == 5.6);
    }

    private static void test_two() throws Exception {
        Calculator parser = new Calculator(new ByteArrayInputStream("33.5 - 2 * 3\n".getBytes()));
        double value = parser.Start(System.out);
        System.out.println("* value: " + value);
        assert(value == 27.5);
    }
    
    private static void test_three() throws Exception {
        Calculator parser = new Calculator(new ByteArrayInputStream("(33.5 - 2) * -3\n".getBytes()));
        double value = parser.Start(System.out);
        System.out.println("* value: " + value);
        assert(value == -94.5);
    }
}

