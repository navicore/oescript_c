import java.io.*;

public class test_javacc_adder {

    public static void main(String[] args) throws Exception {

        assert(true);
        //assert(false);

        test_one();
    }

    private static void test_one() throws Exception {
        System.out.println("testing javacc...");
        Adder parser = new Adder(new ByteArrayInputStream("3 + 4 + 2".getBytes()));
        int value = parser.Start();
        System.out.println("value: " + value);
        assert(value == 9);
    }
}

