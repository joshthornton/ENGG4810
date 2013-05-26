
public enum Mode {
	
	HOLD(0) {
		public String toString() {
			return "hold";
		}
	}, LATCH(1) {
		public String toString() {
			return "latch";
		}
	};
	
	private final int index;
	private Mode( int index )
	{
		this.index = index;
	}
	public int getIndex() {
		return this.index;
	}
	
	public static Mode enumFromIndex( int index )
	{
		switch( index )
		{
		case 1:
			return LATCH;
		default:
			return HOLD;
		}
	}
}
