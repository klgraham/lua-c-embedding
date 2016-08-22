
import java.lang.Throwable;

public class LuaJNI
{
	// loads function defined in scriptPath and returns registry reference
  private native int getFunctionRef(String scriptPath);
	
	// Apply function at Lua registry index ref to n
  private native int apply(int ref, int n);
	private native void startLua();
  private native void closeLua();

  public LuaJNI()
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
    LuaJNI lua = new LuaJNI();
		
    int ref = lua.getFunctionRef("factorial.lua");
    System.out.println("Function ref: " + ref);
		
    for (int m = 0; m < 10; m++)
    {
      int fact = lua.apply(ref, m);
      System.out.println(m + "! = " + fact);
    }
  }
}
