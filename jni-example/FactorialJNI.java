public class FactorialJNI
{
  static {
    System.loadLibrary("factorial");
  }

  private native int factorial(int n);

  public static void main(String[] args)
  {
    int n = Integer.valueOf(args[0]);
    int result = new FactorialJNI().factorial(n);
    System.out.println(n + "! = " + result + "\n");
  }
}
