public class FactorialJNI
{
  private native int factorial(int n);
		
  public static void main(String[] args)
  {
		System.load(args[0]);		
    int n = Integer.valueOf(args[1]);
		
    FactorialJNI jni = new FactorialJNI();
    int result = jni.factorial(n);
		
    System.out.println(n + "! = " + result + "\n");
  }
}
