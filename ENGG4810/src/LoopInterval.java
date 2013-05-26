
public enum LoopInterval {
	
	ONE(0) {
		public String toString() {
			return "1";
		}
	}, TWO(1) {
		public String toString() {
			return "1/2";
		}
	}, FOUR(2) {
		public String toString() {
			return "1/4";
		}
	}, EIGHT(3) {
		public String toString() {
			return "1/8";
		}
	}, SIXTEEN(4) {
		public String toString() {
			return "1/16";
		}
	}, THIRTYTWO(5) {
		public String toString() {
			return "1/32";
		}
	};
	
	private final int index;
	private LoopInterval( int index )
	{
		this.index = index;
	}
	public int getIndex() {
		return this.index;
	}
	
	public int getValue() {
		switch( this )
		{
		case TWO:
			return 2;
		case FOUR:
			return 4;
		case EIGHT:
			return 8;
		case SIXTEEN:
			return 16;
		case THIRTYTWO:
			return 32;
		default:
			return 1;
		}
	}
	
	public static LoopInterval enumFromIndex( int index )
	{
		switch( index )
		{
		case 2:
			return TWO;
		case 4:
			return FOUR;
		case 8:
			return EIGHT;
		case 16:
			return SIXTEEN;
		case 32:
			return THIRTYTWO;
		default:
			return ONE;
		}
	}
}
