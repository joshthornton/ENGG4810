
public enum Action {
	
	NO_ACTION(0) {
		public String toString() {
			return "No Action";
		}
	};
	
	private final int index;
	private Action( int index )
	{
		this.index = index;
	}
	public int getIndex() {
		return this.index;
	}
	
	public static Action enumFromIndex( int index )
	{
		return NO_ACTION;
	}
	
	
}
