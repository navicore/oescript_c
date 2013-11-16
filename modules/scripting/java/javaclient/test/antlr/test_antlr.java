import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;
import java.io.*;

public class test_antlr {

    static String DATA = "one two 3 4\n";

    public static void main(String[] args) throws Exception {

        assert(true);
        //assert(false);

        test_one();
    }

    private static void test_one() throws Exception {
        System.out.println("testing antlr...");
        ANTLRInputStream input = new ANTLRInputStream(new ByteArrayInputStream(DATA.getBytes()));
        ListLexer lexer = new ListLexer(input);
        CommonTokenStream tokens = new CommonTokenStream(lexer);
        ListParser parser = new ListParser(tokens);
        ListParser.r_return result = parser.r();
        Tree t = (Tree) result.getTree();
        System.out.println(t.toStringTree());
    }
}

