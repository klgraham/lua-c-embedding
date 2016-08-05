
import java.lang.Throwable;

public class ComputationJNI
{
  private native int sum(int m);
  private native String factorial(int n);
	private native void startLua();
  private native void closeLua();

  public ComputationJNI()
  {
    this.startLua();
  }

  @Override
  protected void finalize() throws Throwable
  {
    this.closeLua();
  }

  public static void main(String[] args)
  {
    System.load(args[0]);
    int n = Integer.valueOf(args[1]);

    ComputationJNI jni = new ComputationJNI();
		String fact = jni.factorial(n);
    System.out.println(n + "! = " + fact + "\n");
    int sum = -1;

    for (int m = 0; m < 10; m++)
    {
      sum = jni.sum(m);
      System.out.println("Sum(" + m + ", " + fact + ") = " + sum);
    }
  }
}
