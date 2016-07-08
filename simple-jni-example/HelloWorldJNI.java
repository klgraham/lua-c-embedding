public class HelloWorldJNI 
{ 
  private native void hello();
 
  public static void main(String[] args) 
  {
		System.load(args[0]);
    new HelloWorldJNI().hello();  
  }
}
