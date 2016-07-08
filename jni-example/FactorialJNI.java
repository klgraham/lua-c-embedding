public class FactorialJNI
{
  private native int factorial(int n);

  public static void main(String[] args)
  {
		System.load(args[0]);
    int n = Integer.valueOf(args[1]);
    int result = new FactorialJNI().factorial(n);
    System.out.println(n + "! = " + result + "\n");
  }
}
